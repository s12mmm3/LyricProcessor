#ifndef LYRICPROCESSOR_H
#define LYRICPROCESSOR_H

#include "global.h"

#include <QObject>
#include <QVariantMap>

class LyricProcessorPrivate;
class LYRICPROCESSOR_EXPORT LyricProcessor: public QObject
{
    Q_OBJECT
public:
    // Set decimal length for timestamps
    DEFINE_VALUE(int, fixed, 3)
    // Set padding
    DEFINE_VALUE(int, spaceStart, 0)
    DEFINE_VALUE(int, spaceEnd, 0)
    // Set offset
    DEFINE_VALUE(int, offset, 0)
    // Set endOfLine
    DEFINE_VALUE(QString, endOfLine, "\r\n")

public:
    LyricProcessor();
    ~LyricProcessor();

public:
    Q_INVOKABLE void loadLyric(const QString& text);
    Q_INVOKABLE QString getLyric() const;
    Q_INVOKABLE void removeLyric(int index);

    // Load lyric from file
    Q_INVOKABLE void loadFile(const QString& filePath);
    // Save lyric to file
    Q_INVOKABLE bool saveFile(const QString& filePath) const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void clearInfos();
    Q_INVOKABLE void clearLyrics();

    // Set metadata infos
    Q_INVOKABLE void setInfo(const QString& key, const QString& value);
    // Get metadata infos
    Q_INVOKABLE QString getInfo(const QString& key) const;
    Q_INVOKABLE QVariantMap getInfos() const;
    Q_INVOKABLE void removeInfo(const QString& key);

    // Remove all the time tag
    Q_INVOKABLE void removeTags();
    // Remove the time tag at the index
    Q_INVOKABLE void removeTag(int index);
    // Remove all the empty lyric
    Q_INVOKABLE void removeEmpty();

private:
    std::shared_ptr<LyricProcessorPrivate> lPP;
};

#endif // LYRICPROCESSOR_H
