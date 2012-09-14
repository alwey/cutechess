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

#include "pgntagsmodel.h"

const QStringList PgnTagsModel::m_headers = (QStringList() <<
	tr("Name") << tr("Value"));

PgnTagsModel::PgnTagsModel(QObject* parent)
	: QAbstractItemModel(parent)
{
}

void PgnTagsModel::setTags(const QMap<QString, QString>& tags)
{
	m_tags = tags;
	reset();
}

QModelIndex PgnTagsModel::index(int row, int column,
                             const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column);
}

QModelIndex PgnTagsModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index);

	return QModelIndex();
}

int PgnTagsModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_tags.size();
}

int PgnTagsModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_headers.count();
}

QVariant PgnTagsModel::data(const QModelIndex& index, int role) const
{
	if (index.isValid() && role == Qt::DisplayRole)
	{
		if (index.column() == 0)
			return m_tags.keys().at(index.row());
		else if (index.column() == 1)
			return m_tags.values().at(index.row());
	}

	return QVariant();
}

QVariant PgnTagsModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
		return m_headers.at(section);

	return QVariant();
}