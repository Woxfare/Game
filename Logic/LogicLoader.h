/*

     __    __
    / / /\ \ \ ___ _   _   ____ __ _ _ __ ___
    \ \/  \/ /| _ |\ \/ / |  __/ _` | '__/ _ \
     \  /\  / ||_|| >  <  | |_  (_| | | |  __/
      \/  \/  |___|/_/\_\|_  _|\__,_|_|  \___|
                          | |
                          |_|
Copyright (c) 2014 Fernando Perera & Alejandro Vázquez

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef LOGICLOADER_H
#define LOGICLOADER_H

#include <QString>;
#include <map>
#include <Ogre.h>

class QXmlStreamReader;

class LogicLoader
{
public:
    /*!
     * \brief Instance
     * \return
     */
    static LogicLoader* Instance();

    /*!
     * \brief load
     */
    void load();

    /*!
     * \brief clear
     */
    void clear();

    bool isGameLoaded();

private:

    /*!
     * \brief LogicLoader
     */
    LogicLoader();

    /*!
     * \brief _pInstance
     */
    static LogicLoader * _pInstance;

    /*!
     * \brief loadTextures
     * \param xml
     */
    void loadTextures( QXmlStreamReader& xml );

    /*!
     * \brief loadPlayer
     * \param xml
     */
    void loadPlayer( QXmlStreamReader& xml );

    /*!
     * \brief loadPlayer
     * \param xml
     */
    void loadEnemyType(QXmlStreamReader& xml);

    /*!
     * \brief loadItem
     * \param xml
     */
    void loadItem( QXmlStreamReader& xml );

    void loadWeapons( QXmlStreamReader& xml );

    void loadDynamicObjects(QXmlStreamReader& xml);

    /*!
     * \brief loadMap
     * \param xml
     */
    void loadMap( QXmlStreamReader& xml );

    /*!
     * \brief loadGame
     * \param xml
     */
    void loadGame(QXmlStreamReader &xml);

    /*!
     * \brief loadDynamicObject
     * \param xml
     */
    void loadDynamicObject(QXmlStreamReader &xml);

    /*!
     * \brief parseColor
     * \param xml
     * \param r
     * \param g
     * \param b
     */
    void parseColor( QXmlStreamReader& xml, int& r, int& g, int& b );

    /*!
     * \brief parseCharacterTextures
     * \param xml
     * \param ar_textures
     */
    void parseCharacterTextures( QXmlStreamReader& xml, std::map<std::string, std::map<std::string, std::string>>& ar_textures );

    /*!
     * \brief parseCubeTextures
     * \param xml
     * \param ar_textures
     */
    void parseCubeTextures( QXmlStreamReader& xml, std::map<std::string, std::string>& ar_textures );

    void copyFile(QString path, QString filenameSource, QString filenameDest );

    /*!
     * \brief gameLoaded
     */
    bool gameLoaded;

    /*!
     * \brief _path_cnf_file
     */
    QString _path_cnf_file;
};

#endif // LOGICLOADER_H
