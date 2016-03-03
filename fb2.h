/*
This file is part of kde-thumbnailer-fb2
Copyright (C) 2016 Giacomo Barazzetti <giacomosrv@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FB2_H
#define FB2_H

#include <QtCore/QFile>
#include <kzip.h>

namespace fb2 {
    
const QString FB2_EXT = "fb2";
const QString FB2ZIP_EXT = "zip";

class OpenStrategy
{    
    public:
        virtual ~OpenStrategy() { }
        virtual QIODevice* createDevice() = 0;
};

class OpenFb2 : public OpenStrategy
{
    private:
        QFile *file;
        
    public:
        OpenFb2(const QString &path);
        virtual ~OpenFb2();
        QIODevice* createDevice();
};

class OpenZippedFb2 : public OpenStrategy
{
    private:
        KZip zip;
        QIODevice *device;
        
    public:
        OpenZippedFb2(const QString &path);
        virtual ~OpenZippedFb2();
        QIODevice* createDevice();
};

}
#endif // FB2_H
