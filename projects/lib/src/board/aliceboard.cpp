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

#include "aliceboard.h"
#include "westernzobrist.h"
#include "boardtransition.h"
#include <qabstractitemmodel.h>

namespace Chess {

AliceBoard::AliceBoard()
	: RestrictedMoveBoard(),
	  m_width(8),
	  m_arwidth(18),
	  m_kingIndex{0, 0}
{
}

Board* AliceBoard::copy() const
{
	return new AliceBoard(*this);
}

QString AliceBoard::variant() const
{
	return "alice";
}

QString AliceBoard::defaultFenString() const
{
	return "rnbqkbnr8/pppppppp8/16/16/16/16/PPPPPPPP8/RNBQKBNR8 w KQkq - 0 1";
}

int AliceBoard::width() const
{
	return 2 * 8;
}

void AliceBoard::vInitialize()
{
	RestrictedMoveBoard::vInitialize();
	// width of (half) board
	m_width = width() / 2;
	m_arwidth = 2 * m_width + 2;
}

bool AliceBoard::vSetFenString(const QStringList& fen)
{
	bool ret = RestrictedMoveBoard::vSetFenString(fen);
	m_kingIndex[Side::White] = kingSquare(Side::White);
	m_kingIndex[Side::Black] = kingSquare(Side::Black);
	return ret;
}

int AliceBoard::castlingFile(RestrictedMoveBoard::CastlingSide castlingSide) const
{
	return castlingSide == QueenSide ? 2 : 6;
}

inline int AliceBoard::boardId(int index) const
{
	return index % m_arwidth > m_width;
}

inline bool AliceBoard::withinBoard(const Move& move) const
{
	return boardId(move.sourceSquare()) == boardId(move.targetSquare());
}

bool AliceBoard::restriction(const Move& move, bool) const
{
	return withinBoard(move);
}

inline int AliceBoard::mirrored(int index) const
{
	return boardId(index) ? index - m_width : index + m_width;
}

inline Move AliceBoard::mirrored(const Move& move) const
{
	return Move(mirrored(move.sourceSquare()),
		    mirrored(move.targetSquare()),
		    move.promotion());
}

inline int AliceBoard::toBoardA(int index) const
{
	return boardId(index) ? index - m_width : index;
}

inline Move AliceBoard::toBoardA(const Move& move) const
{
	return Move(toBoardA(move.sourceSquare()),
		    toBoardA(move.targetSquare()),
		    move.promotion());
}

bool AliceBoard::isLegalPosition()
{
	return RestrictedMoveBoard::isLegalPosition();
}


bool AliceBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		square = m_kingIndex[side];

	return RestrictedMoveBoard::inCheck(side, square);
}

bool AliceBoard::vIsLegalMove(const Move& move)
{
	if (kingSquare(sideToMove()) != m_kingIndex[sideToMove()])
		  qDebug() << kingSquare(sideToMove()) << "  " << m_kingIndex[sideToMove()] << "  " << lanMoveString(move);
	// test legality of complete move
	// RestrictedMoveBoard prevents direct move between boards
	if (!RestrictedMoveBoard::vIsLegalMove(move))
		return false;

	// test legality within the board
	Side side = sideToMove();
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece(pieceAt(source));
	// King must not stay in check on this board
	// TODO: castling targets, new empty squares for Rook, King
	if (piece == Piece(side, King))
	{
		// prevent "self-hiding" from checks
		setSquare(source, Piece());
		bool check = inCheck(side, target);
		setSquare(source, piece);
		if (check)
			return false;
	}
	// necessary tests in case of imported moves from external sources
	// corresponding square on other board must be empty
	int newTarget = mirrored(target);
	return pieceAt(newTarget).isEmpty();
}

void AliceBoard::vMakeMove(const Move& move, BoardTransition* transition)
{
	int source = move.sourceSquare();
	int target = move.targetSquare();
	Piece piece0(pieceAt(source));

	RestrictedMoveBoard::vMakeMove(move, transition);

	// kingSquare already updated: gives correct target even for castling
	if (piece0.type() == King)
	      target = kingSquare(sideToMove());

	Piece piece(pieceAt(target));
	int newTarget = mirrored(target);
	setSquare(target, Piece::NoPiece);
	setSquare(newTarget, piece);

	Square newSq = chessSquare(newTarget);
	if (transition != nullptr)
		transition->addSquare(newSq);

	if (piece.type() != King)
		return;

	m_kingIndex[piece.side()] = newTarget;
	if (target == move.targetSquare())
		return;

	// castling: transfer rook to other board
	int rookIdx = target + (target < move.targetSquare() ? -1 : 1);
	int mrookIdx = mirrored(rookIdx);
	Q_ASSERT(pieceAt(mrookIdx).isEmpty());
	setSquare(mrookIdx, Piece(pieceAt(rookIdx)));
	setSquare(rookIdx, Piece::NoPiece);
	Q_ASSERT(pieceAt(mrookIdx) == Piece(sideToMove(), Rook));

	if (transition != nullptr)
		transition->addSquare(chessSquare(mrookIdx));
}

void AliceBoard::vUndoMove(const Move& move)
{
	int target = move.targetSquare();
	int newTarget = mirrored(target);
	Side side = sideToMove();
	Piece piece(pieceAt(newTarget));

	// reconstruct castling move
	if (piece == Piece::NoPiece)
	{
		newTarget = m_kingIndex[side];
		target = mirrored(newTarget);
		piece = Piece(pieceAt(newTarget));
		Q_ASSERT(piece == Piece(side, King));

		// castling: transfer rook back to other board
		int rookIdx = newTarget + (target < move.targetSquare() ? -1 : 1);
		int mrookIdx = mirrored(rookIdx);
		Q_ASSERT(pieceAt(mrookIdx).isEmpty());
		setSquare(mrookIdx, Piece(pieceAt(rookIdx)));
		setSquare(rookIdx, Piece::NoPiece);
		Q_ASSERT(pieceAt(mrookIdx) == Piece(side, Rook));
	}

	setSquare(newTarget, Piece::NoPiece);
	setSquare(target, piece);

	RestrictedMoveBoard::vUndoMove(move);

	if (piece.type() == King)
		m_kingIndex[piece.side()] = move.sourceSquare();
}

void AliceBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
				       int pieceType,
				       int square) const
{
	QVarLengthArray< Move > moves1;

	RestrictedMoveBoard::generateMovesForPiece(moves1, pieceType, square);

	for (const Move& m: moves1)
	{
		// move must stay within (half) board
		// and square on mirror board must be free
		if (!withinBoard(m)
		||  !pieceAt(mirrored(m.targetSquare())).isEmpty())
			continue;

		// additional checks for castling moves
		Side side = sideToMove();
		int target = m.targetSquare();
		if (pieceType == King
		&&  pieceAt(target) == Piece(side, Rook))
		{
			int kTarget = square / m_arwidth * m_arwidth + 1;
			kTarget += (square < target) ? castlingFile(KingSide)
						     : castlingFile(QueenSide);
			int rTarget = kTarget + ((square < target) ? -1 : 1);
			if (!pieceAt(mirrored(kTarget)).isEmpty()
			||  !pieceAt(mirrored(rTarget)).isEmpty())
				continue;
		}
		moves.append(m);
	}
}

bool AliceBoard::oneBoardOutputFormat() const
{
	return false;
}

QString AliceBoard::lanMoveString(const Move& move)
{
	if (oneBoardOutputFormat())
		return RestrictedMoveBoard::lanMoveString(move);

	Move move1 = toBoardA(move);
	return RestrictedMoveBoard::lanMoveString(move1);
}

QString AliceBoard::sanMoveString(const Move& move)
{
	QString s = RestrictedMoveBoard::sanMoveString(move);

	if (oneBoardOutputFormat()
	||  move == toBoardA(move))
		return s;

	// move on board B: replace target square
	int target = move.targetSquare();
	QString tgt = squareString(target);
	int mirr = mirrored(target);
	QString mtgt = squareString(mirr);
	s.replace(tgt, mtgt);

	// replace source file if necessary
	int source = move.sourceSquare();
	Piece pc(pieceAt(source));
	QString symbol = (pc.type() != Pawn) ? pieceSymbol(pc) : "";
	int msource = mirrored(source);
	QString sfile = squareString(source).left(1);
	QString mfile = squareString(msource).left(1);
	s.replace(symbol + sfile, symbol + mfile);

	// piece appears on Board A
	return s + "/A";
}

Move AliceBoard::moveFromLanString(const QString& str)
{
	Move move = RestrictedMoveBoard::moveFromLanString(str);
	if (pieceAt(move.sourceSquare()) != Piece::NoPiece)
		return move;

	Move move1 = mirrored(move);
	return move1;
}

Move AliceBoard::moveFromSanString(const QString& istr)
{
	QString str(istr);
	const QStringList patterns{"/A",">A","(A)","/B",">B","(B)"};
	for (const QString& sub: patterns)
		if (str.contains(sub))
			str.remove(sub);

	Move move = RestrictedMoveBoard::moveFromSanString(str);
	if (move != Move())
		return move;

	// transformation of 8x8 to 16x8 internal board
	for (QChar c: str)
		if ( QString("abcdefgh").contains(c))
			str.replace(c, c.toLatin1() + m_width);

	move = RestrictedMoveBoard::moveFromSanString(str);
	return move;
}

} // namespace Chess
