/*
    Copyright 2018, simba wei.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H
#include <string>
#include <string.h>
class ThreadException
{
public:
    typedef std::string string;
public:
    ThreadException(const string &_msg = string(),int errRet = 0);
    string what() const;
private:
    string msg_;
};

ThreadException::ThreadException(const string &_msg, int errRet)
    :msg_(_msg)
{
    msg_ += strerror(errRet);
}

std::string ThreadException::what() const
{
    return msg_;
}

#endif // THREADEXCEPTION_H
