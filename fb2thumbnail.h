/*
This file is part of kde-thumbnailer-fb2
Copyright (C) 2013-2016 Giacomo Barazzetti <giacomosrv@gmail.com>

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

#ifndef FB2THUMBNAIL_H
#define FB2THUMBNAIL_H

#include <QtCore/QObject>
#include <kio/thumbcreator.h>

class Fb2Creator : public QObject, public ThumbCreator
{
    Q_OBJECT
     
    public:
        virtual bool create(const QString &path, int width, int height, QImage &img);
        virtual Flags flags() const;
};

#endif // FB2THUMBNAIL_H
