/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2020 Cute Chess authors

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

#ifndef ARMAGEDDONBOARD_H
#define ARMAGEDDONBOARD_H

#include "standardboard.h"

namespace Chess {

/*!
 * \brief A board for Armageddon Chess
 *
 * Armageddon is standard chess, however drawn games are adjudicated as
 * a win for Black.
 *
 * \note Rules: https://en.wikipedia.org/wiki/Fast_chess#Armageddon
 * */
class LIB_EXPORT ArmageddonBoard : public StandardBoard
{
	public:
		/*! Creates a new ArmageddonBoard object. */
		ArmageddonBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual Result result();
};

/*!
 * \brief A board for Reverse Armageddon Chess
 *
 * Reverse Armageddon is standard chess, however drawn games are adjudicated as
 * a win for White.
 *
 * */
class LIB_EXPORT ReverseArmageddonBoard : public StandardBoard
{
	public:
		/*! Creates a new ReverseArmageddonBoard object. */
		ReverseArmageddonBoard();

		// Inherited from StandardBoard
		virtual Board* copy() const;
		virtual QString variant() const;
		virtual Result result();
};

} // namespace Chess
#endif // ARMAGEDDONBOARD_H
