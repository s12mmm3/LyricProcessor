#include "lyricprocessor.h"

#include <QString>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>

struct Lyric {
    qreal     time;
    QString   text;
};

using LyricList   = QList<Lyric>;
using infoMap     = QVariantMap;

class LyricProcessorUtil {
public:
    static QString convertTimeToTag(qreal time, int fixed = 3, int offset = 0, bool withBrackets = true) {
        double c = offset / -1000;
        if (qIsNaN(c)) c = 0;
        time = time + c;

        int minutes = static_cast<int>(time) / 60;
        int seconds = static_cast<int>(time) - minutes * 60;
        qreal milliseconds = time - static_cast<int>(time);

        const QString mm =
            QString("%1").arg(minutes, 2, 10, QLatin1Char('0'));
        const QString ss =
            QString("%1").arg(static_cast<int>(seconds), 2, 10, QLatin1Char('0'));
        const QString ms =
            QString::asprintf(("%." + QString::number(fixed) + "f").toStdString().c_str(), milliseconds).remove(0, 2);

        const QString formattedTime = QString(withBrackets ? "[%1:%2.%3]" : "%1:%2.%3").arg(mm, ss, ms);

        return formattedTime;
    }

    static QString formatText(QString text, int spaceStart, int spaceEnd) {
        auto newText = text;
        if (spaceStart >= 0) {
            newText = QString(spaceStart, ' ') + newText.trimmed();
        }
        if (spaceEnd >= 0) {
            newText = newText.trimmed() + QString(spaceEnd, ' ');
        }
        return newText;
    }

    static QPair<infoMap, LyricList> parser(const QString& lrcString) {
        static QRegularExpression splitReg("\\r\\n|\\n|\\r");
        QStringList lines = lrcString.split(splitReg);

        static QRegularExpression timeTag(R"(\[\s*(\d{1,3}):(\d{1,2}(?:[:.]\d{1,3})?)\s*])");
        static QRegularExpression infoTag(R"(\[\s*(\w{1,6})\s*:(.*?)])");

        infoMap info;
        LyricList lyric;

        QRegularExpressionMatch match;
        foreach (const QString& line, lines) {
            // Check if line starts with "["
            if (!line.startsWith('[')) {
                lyric.append({ NAN, line });
                continue;
            }

            // Check for time tag
            match = timeTag.match(line);
            if (match.hasMatch()) {
                int mm = match.captured(1).toInt();
                qreal ss = match.captured(2).replace(':', '.').toDouble();
                QString text = line.mid(match.capturedLength());

                lyric.append({ mm * 60 + ss, text });
                continue;
            }

            // Check for info tag
            match = infoTag.match(line);
            if (match.hasMatch()) {
                QString value = match.captured(2).trimmed();
                if (!value.isEmpty()) {
                    info.insert(match.captured(1), value);
                }
                continue;
            }

            // If we reach here, it means this line starts with "[", but does not match any known tags.
            lyric.append({ NAN, line });
        }
        return { info, lyric };
    }
};

class LyricProcessorPrivate {
public:
    LyricList m_lyric; // Store parsed lyrics
    infoMap m_info; // Store metadata
};


using LPUtil   = LyricProcessorUtil;
using LPP      = LyricProcessorPrivate;
using LP       = LyricProcessor;

LP::LyricProcessor(): lPP(std::make_shared<LPP>()) { }

LP::~LyricProcessor() { }

void LP::loadLyric(const QString& text) {
    auto pair = LPUtil::parser(text);
    lPP->m_info = std::move(pair.first);
    lPP->m_lyric = std::move(pair.second);
}

QString LP::getLyric() const {
    // Output metadata
    QString infos;
    for (auto i = lPP->m_info.constBegin(); i != lPP->m_info.constEnd(); i++) {
        infos += QString("[%1: %2]" + endOfLine())
                     .arg(i.key(), i.value().toString());
    }

    // Output lyrics
    QString lines;
    for (auto i = lPP->m_lyric.constBegin(); i != lPP->m_lyric.constEnd(); i++) {
        if (qIsNaN(i->time)) {
            lines += i->text;
        }
        else {
            const auto text = LPUtil::formatText(i->text, spaceStart(), spaceEnd());
            lines += LPUtil::convertTimeToTag(i->time, fixed(), offset()) + text;
        }
        lines += endOfLine();
    }
    return infos + lines;
}

void LyricProcessor::removeLyric(int index) {
    lPP->m_lyric.remove(index);
}

void LP::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        loadLyric(file.readAll());
    }
    else {
        qDebug() << "Failed to open file: " << filePath << ". Error: " << file.errorString();
    }
}

bool LP::saveFile(const QString& filePath) const {
    QFile file(filePath);
    if (file.open(QFile::ReadOnly)) {
        file.write(getLyric().toUtf8());
        return true;
    }
    else {
        qDebug() << "Failed to open file: " << filePath << " for writing. Error: " << file.errorString();
        return false;
    }
}

void LyricProcessor::clear() {
    clearInfos();
    clearLyrics();
}

void LyricProcessor::clearInfos() {
    lPP->m_info.clear();
}

void LyricProcessor::clearLyrics() {
    lPP->m_lyric.clear();
}

void LP::setInfo(const QString& key, const QString& value) {
    lPP->m_info.insert(key, value);
}

QString LP::getInfo(const QString& key) const {
    return lPP->m_info.value(key, "").toString();
}

QVariantMap LyricProcessor::getInfos() const {
    return lPP->m_info;
}

void LyricProcessor::removeInfo(const QString &key) {
    lPP->m_info.remove(key);
}

void LyricProcessor::removeTags() {
    for (auto& l: lPP->m_lyric) {
        l = { NAN, l.text };
    }
}

void LyricProcessor::removeTag(int index) {
    if (lPP->m_lyric.size() > index + 1) {
        auto l = lPP->m_lyric[index];
        lPP->m_lyric[index] = { NAN, l.text };
    }
}

void LyricProcessor::removeEmpty() {
    for (auto iter = lPP->m_lyric.constBegin(); iter != lPP->m_lyric.constEnd(); iter++) {
        if (iter->text.trimmed().isEmpty()) {
            lPP->m_lyric.erase(iter);
        }
    }
}
