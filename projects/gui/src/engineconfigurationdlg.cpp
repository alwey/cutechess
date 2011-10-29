/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "engineconfigurationdlg.h"
#include "ui_engineconfigdlg.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

#include <enginefactory.h>
#include <engineoption.h>
#include <chessplayer.h>
#include <enginebuilder.h>

#include "engineoptionmodel.h"
#include "engineoptiondelegate.h"

EngineConfigurationDialog::EngineConfigurationDialog(
	EngineConfigurationDialog::DialogMode mode, QWidget* parent)
	: QDialog(parent),
	  m_engineOptionModel(new EngineOptionModel(this)),
	  ui(new Ui::EngineConfigurationDialog)
{
	ui->setupUi(this);

	if (mode == EngineConfigurationDialog::AddEngine)
		setWindowTitle(tr("Add Engine"));
	else
		setWindowTitle(tr("Configure Engine"));

	ui->m_protocolCombo->addItems(EngineFactory::protocols());

	ui->m_optionsView->setModel(m_engineOptionModel);
	ui->m_optionsView->setItemDelegate(new EngineOptionDelegate());

	m_optionDetectionTimer = new QTimer(this);
	m_optionDetectionTimer->setSingleShot(true);
	m_optionDetectionTimer->setInterval(5000);

	connect(ui->m_browseCmdBtn, SIGNAL(clicked(bool)),
		this, SLOT(browseCommand()));
	connect(ui->m_browseWorkingDirBtn, SIGNAL(clicked(bool)),
		this, SLOT(browseWorkingDir()));
	connect(ui->m_detectBtn, SIGNAL(clicked()),
		this, SLOT(detectEngineOptions()));
	connect(ui->m_tabs, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)));
	connect(ui->m_buttonBox, SIGNAL(accepted()),
		this, SLOT(onAccepted()));
}

EngineConfigurationDialog::~EngineConfigurationDialog()
{
	qDeleteAll(m_options);
	delete ui;
}

void EngineConfigurationDialog::applyEngineInformation(
	const EngineConfiguration& engine)
{
	ui->m_nameEdit->setText(engine.name());
	ui->m_commandEdit->setText(engine.command());
	ui->m_workingDirEdit->setText(engine.workingDirectory());

	int i = ui->m_protocolCombo->findText(engine.protocol());
	ui->m_protocolCombo->setCurrentIndex(i);

	ui->m_initStringEdit->setPlainText(engine.initStrings().join("\n"));

	if (engine.whiteEvalPov())
		ui->m_whitePovCheck->setCheckState(Qt::Checked);

	foreach (EngineOption* option, engine.options())
		m_options << option->copy();
	m_engineOptionModel->setOptions(m_options);

	m_variants = engine.supportedVariants();

	m_oldCommand = engine.command();
	m_oldPath = engine.workingDirectory();
	m_oldProtocol = engine.protocol();
}

EngineConfiguration EngineConfigurationDialog::engineConfiguration()
{
	EngineConfiguration engine;
	engine.setName(ui->m_nameEdit->text());
	engine.setCommand(ui->m_commandEdit->text());
	engine.setWorkingDirectory(ui->m_workingDirEdit->text());
	engine.setProtocol(ui->m_protocolCombo->currentText());

	QString initStr(ui->m_initStringEdit->toPlainText());
	if (!initStr.isEmpty())
		engine.setInitStrings(initStr.split('\n'));

	engine.setWhiteEvalPov(ui->m_whitePovCheck->checkState() == Qt::Checked);

	QList<EngineOption*> optionCopies;
	foreach (EngineOption* option, m_options)
		optionCopies << option->copy();

	engine.setOptions(optionCopies);

	engine.setSupportedVariants(m_variants);

	return engine;
}

void EngineConfigurationDialog::browseCommand()
{
	// Use file extensions only on Windows
	#ifdef Q_WS_WIN
	const QString filter = tr("Executables (*.exe *.bat *.cmd);;All Files (*.*)");
	#else
	const QString filter = tr("All Files (*)");
	#endif

	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Select Engine Executable"), ui->m_commandEdit->text(), filter);

	if (fileName.isEmpty())
		return;

	if (ui->m_workingDirEdit->text().isEmpty())
		ui->m_workingDirEdit->setText(QDir::toNativeSeparators(
			QFileInfo(fileName).absolutePath()));

	if (ui->m_nameEdit->text().isEmpty())
		ui->m_nameEdit->setText(QFileInfo(fileName).baseName());

	// Paths with spaces must be wrapped in quotes
	if (fileName.contains(' '))
	{
		fileName.prepend('\"');
		fileName.append('\"');
	}
	ui->m_commandEdit->setText(QDir::toNativeSeparators(fileName));
}

void EngineConfigurationDialog::browseWorkingDir()
{
	const QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Engine Working Directory"), ui->m_workingDirEdit->text());

	if (directory.isEmpty())
		return;

	ui->m_workingDirEdit->setText(QDir::toNativeSeparators(directory));
}

void EngineConfigurationDialog::detectEngineOptions()
{
	if (m_optionDetectionTimer->isActive())
		return;

	if (QObject::sender() != ui->m_detectBtn
	&&  ui->m_commandEdit->text() == m_oldCommand
	&&  ui->m_workingDirEdit->text() == m_oldPath
	&&  ui->m_protocolCombo->currentText() == m_oldProtocol)
	{
		emit detectionFinished();
		return;
	}

	m_oldCommand = ui->m_commandEdit->text();
	m_oldPath = ui->m_workingDirEdit->text();
	m_oldProtocol = ui->m_protocolCombo->currentText();

	EngineBuilder builder(engineConfiguration());
	ChessPlayer* engine = builder.create(0, 0, this);

	if (engine != 0)
	{
		connect(engine, SIGNAL(ready()),
			this, SLOT(onEngineReady()));
		connect(engine, SIGNAL(disconnected()),
			engine, SLOT(deleteLater()));
		connect(engine, SIGNAL(destroyed()),
			this, SIGNAL(detectionFinished()));
		connect(m_optionDetectionTimer, SIGNAL(timeout()),
			engine, SLOT(quit()));

		ui->m_detectBtn->setEnabled(false);
		m_optionDetectionTimer->start();
	}
	else
		emit detectionFinished();
}

void EngineConfigurationDialog::onEngineReady()
{
	ChessEngine* engine = qobject_cast<ChessEngine*>(QObject::sender());
	Q_ASSERT(engine != 0);

	disconnect(m_optionDetectionTimer, 0, engine, 0);
	m_optionDetectionTimer->stop();

	ui->m_detectBtn->setEnabled(true);

	if (engine->state() == ChessPlayer::Disconnected)
		return;

	qDeleteAll(m_options);
	m_options.clear();

	// Make copies of the engine options
	foreach (const EngineOption* option, engine->options())
		m_options << option->copy();

	m_engineOptionModel->setOptions(m_options);
	m_variants = engine->variants();

	engine->quit();
}

void EngineConfigurationDialog::onTabChanged(int index)
{
	if (index == 1)
		detectEngineOptions();
}

void EngineConfigurationDialog::onAccepted()
{
	connect(this, SIGNAL(detectionFinished()),
		this, SLOT(accept()));
	detectEngineOptions();
}
