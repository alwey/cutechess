/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#include "knightrelayboard.h"
#include "westernzobrist.h"

namespace Chess {

KnightRelayBoard::KnightRelayBoard()
	: RestrictedMoveBoard()
{
}

Board* KnightRelayBoard::copy() const
{
	return new KnightRelayBoard(*this);
}

QString KnightRelayBoard::variant() const
{
	return "knightrelay";
}

QString KnightRelayBoard::defaultFenString() const
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

bool KnightRelayBoard::hasEnPassantCaptures() const
{
	return false;
}

void KnightRelayBoard::vInitialize()
{
	RestrictedMoveBoard::vInitialize();

	int arwidth = width() + 2;
	m_knightOffsets.resize(8);
	m_knightOffsets[0] = -2 * arwidth - 1;
	m_knightOffsets[1] = -2 * arwidth + 1;
	m_knightOffsets[2] = -arwidth - 2;
	m_knightOffsets[3] = -arwidth + 2;
	m_knightOffsets[4] = arwidth - 2;
	m_knightOffsets[5] = arwidth + 2;
	m_knightOffsets[6] = 2 * arwidth - 1;
	m_knightOffsets[7] = 2 * arwidth + 1;
}

bool KnightRelayBoard::defendedByKnight(int square, Side side) const
{
	// Find a defending Knight
	for (int offset : m_knightOffsets)
	{
		Piece piece = pieceAt(square + offset);
		if (piece.type() == Knight
		&&  piece.side() == side)
			return true;
	}
	return false;
}

void KnightRelayBoard::generateMovesForPiece(QVarLengthArray<Move>& moves,
					     int pieceType,
					     int square) const
{
	QVarLengthArray<Move> moves1;
	if (pieceType == Knight)
	{
		RestrictedMoveBoard::generateMovesForPiece(moves1, Knight, square);
		// Only add non-capturing moves for Knights
		for (const Move& m: moves1)
			if (captureType(m) == Piece::NoPiece)
				moves.append(m);
	}
	else
	// Normal moves
		RestrictedMoveBoard::generateMovesForPiece(moves, pieceType, square);

	// Add Knight moves if defended by Knight
	if (!pieceHasMovement(pieceType, KnightMovement)
	&&  defendedByKnight(square, sideToMove()))
		RestrictedMoveBoard::generateMovesForPiece(moves, Knight, square);
}

bool KnightRelayBoard::inCheck(Side side, int square) const
{
	if (square == 0)
		square = kingSquare(side);

	// Find a relayed Knight attack
	Side opp = side.opposite();
	for (int offset : m_knightOffsets)
	{
		int index = square + offset;
		Piece piece = pieceAt(index);
		if (piece.type() != Knight
		&&  piece.side() == opp
		&&  defendedByKnight(index, opp))
		{
			// Exclude pawn leaps to the first and last ranks
			int rank = chessSquare(square).rank();
			if (piece.type() != Pawn
			||  (rank > 0 && rank < height() - 1))
				return true;
		}
	}
	return RestrictedMoveBoard::inCheck(side, square);
}

bool KnightRelayBoard::vIsLegalMove(const Chess::Move& move)
{
	// Knights cannot capture
	int source = move.sourceSquare();
	if (pieceAt(source).type() == Knight
	&&  captureType(move) != Piece::NoPiece)
		return false;

	// Knights cannot be captured
	if (captureType(move) == Knight)
		return false;

	// Pawns cannot leap to the first and last ranks
	Square tgtSq = chessSquare(move.targetSquare());
	int rank = tgtSq.rank();

	if (pieceAt(source).type() == Pawn
	&&  move.promotion() == 0
	&&  (rank == height() - 1 || rank == 0))
		return false;

	return RestrictedMoveBoard::vIsLegalMove(move);
}

bool KnightRelayBoard::restriction(const Move& m, bool reverse) const
{
	return true;
}

} // namespace Chess
