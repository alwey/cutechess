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

#ifndef KNIGHRELAYBOARD_H
#define KNIGHRELAYBOARD_H

#include "restrictedmoveboard.h"

namespace Chess {

/*!
 * \brief A board for Knight-Relay Chess
 *
 * Knight-Relay is a variant of standard chess where pieces
 * defended by a friendly Knight gain the power of a Knight.
 * Knights cannot capture (or check) or be captured; Pawns
 * cannot capture en passant, or be relayed to the first or
 * last ranks.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Knight_relay_chess
 *
 * */
class LIB_EXPORT KnightRelayBoard : public RestrictedMoveBoard
{
	public:
		/*! Creates a new KnightRelayBoard object. */
		KnightRelayBoard();

		// Inherited from RestrictedMoveBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;

	protected:
		// Inherited from RestrictedMoveBoard
		virtual void vInitialize();
		virtual bool hasEnPassantCaptures() const;
		virtual bool vIsLegalMove(const Chess::Move & move);
		virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
						   int pieceType,
						   int square) const;
		virtual bool inCheck(Chess::Side side, int square) const;
		virtual bool restriction(const Move& move,
					 bool reverse = false) const;
	private:
		bool defendedByKnight(int square, Side side) const;
		QVarLengthArray<int> m_knightOffsets;
};

} // namespace Chess
#endif // KNIGHRELAYBOARD_H
