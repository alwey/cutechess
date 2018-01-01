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

#ifndef ALICEBOARD_H
#define ALICEBOARD_H

#include "restrictedmoveboard.h"

namespace Chess {

/*!
 * \brief A board for Alice Chess (a.k.a. Alician Chess, Looking Glass)
 *
 * Alice Chess is a variant of standard chess which uses two boards.
 * The game starts from a standard setup on the first board.
 * A piece makes a legal move on one board and then transfers to the
 * corresponding empty square of the other board. If this square is
 * not empty then the move is forbidden.
 *
 * Introduced by Vernon R. Parton, UK, in 1953.
 * Named after Lewis Carroll's character Alice.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Alice_Chess
 *
 * TODO: checks, castling checks, game adjudication
 * */
class LIB_EXPORT AliceBoard : public RestrictedMoveBoard
{
	public:
		/*! Creates a new AliceBoard object. */
		AliceBoard();

		// Inherited from RestrictedMoveBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual QString defaultFenString() const;
		virtual int width() const;

	protected:
		// Inherited from RestrictedMoveBoard
		virtual void vInitialize();
		virtual int castlingFile(CastlingSide castlingSide) const;
		virtual bool restriction(const Move& move,
					 bool reverse = false) const;
		virtual bool vSetFenString(const QStringList& fen);
		virtual QString lanMoveString(const Move& move);
		virtual QString sanMoveString(const Move& move);
		virtual Move moveFromLanString(const QString& str);
		virtual Move moveFromSanString(const QString& str);
		virtual bool isLegalPosition();
		virtual bool inCheck(Side side,
				     int square = 0) const;
		virtual bool vIsLegalMove(const Move& move);
		virtual void vMakeMove(const Move& move,
				       BoardTransition* transition);
		virtual void vUndoMove(const Move& move);
		virtual void generateMovesForPiece(QVarLengthArray< Move >& moves,
						   int pieceType,
						   int square) const;


	private:
		int m_width;
		int m_arwidth;
		int m_kingIndex[2];
		int boardId(int index) const;

		bool withinBoard(const Move& move) const;
		/*!
		 * Returns corresponding square index for \a index on
		 * mirror (half) board.
		 */
		inline int mirrored(int index) const;
		inline Move mirrored(const Move& move) const;
		inline int toBoardA(int index) const;
		inline Move toBoardA(const Move& move) const;
		inline bool oneBoardOutputFormat() const;
};

} // namespace Chess
#endif // ALICEBOARD_H
