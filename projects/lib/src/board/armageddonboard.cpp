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

#include "armageddonboard.h"

namespace Chess {

ArmageddonBoard::ArmageddonBoard()
	: StandardBoard()
{
}

Board* ArmageddonBoard::copy() const
{
	return new ArmageddonBoard(*this);
}

QString ArmageddonBoard::variant() const
{
	return "armageddon";
}

Result ArmageddonBoard::result()
{
	Result result = StandardBoard::result();
	if (result.isDraw())
	{
		Side winner = Side::Black;
		QString str = tr("Draw odds: %1 wins").arg(winner.toString());;
		return Result(Result::Type::Win, winner, str);
	}
	return result;
}


ReverseArmageddonBoard::ReverseArmageddonBoard()
	: StandardBoard()
{
}

Board* ReverseArmageddonBoard::copy() const
{
	return new ReverseArmageddonBoard(*this);
}

QString ReverseArmageddonBoard::variant() const
{
	return "reversearmageddon";
}

Result ReverseArmageddonBoard::result()
{
	Result result = StandardBoard::result();
	if (result.isDraw())
	{
		Side winner = Side::White;
		QString str = tr("Draw odds: %1 wins").arg(winner.toString());;
		return Result(Result::Type::Win, winner, str);
	}
	return result;
}

} // namespace Chess
