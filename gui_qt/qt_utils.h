/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <sstream>
#include <string>
#include <QByteArray>
#include <QImage>
#include <QCryptographicHash>
#include <QFileInfo>
#include <opencv2/opencv.hpp>

QImage GrayMat2QImage(cv::Mat const& src);
QImage Mat2QImage(cv::Mat const& src);
cv::Mat QImage2Mat(QImage const& src);

QString base64_encode(const QString string);
QString base64_decode(const QString string);
QString md5_encode(const QString);

int sti(const std::string &s);
std::string its(int i);

bool fileExists(QString path);

/*
#include <QFileDialog>
#include <QApplication>
#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QStringList>
#include <QModelIndex>
#include <QDir>
#include <QDebug>

class FileDialog : public QFileDialog
{
    Q_OBJECT
    public:
        explicit FileDialog(QWidget *parent = Q_NULLPTR)
            : QFileDialog(parent)
        {
            setOption(QFileDialog::DontUseNativeDialog);
            setFileMode(QFileDialog::Directory);
            //setFileMode(QFileDialog::ExistingFiles);
            //setFileMode(QFileDialog::Directory|QFileDialog::ExistingFiles);
            for (auto *pushButton : findChildren<QPushButton*>()) {
                qDebug() << pushButton->text();
                if (pushButton->text() == "&Open" || pushButton->text() == "&Choose") {
                    openButton = pushButton;
                    break;
                }
            }
            disconnect(openButton, SIGNAL(clicked(bool)));
            connect(openButton, &QPushButton::clicked, this, &FileDialog::openClicked);
            treeView = findChild<QTreeView*>();
        }

        QStringList selected() const
        {
            return selectedFilePaths;
        }

    public slots:
        void openClicked()
        {
            selectedFilePaths.clear();
            qDebug() << treeView->selectionModel()->selection();
            for (const auto& modelIndex : treeView->selectionModel()->selectedIndexes()) {
                qDebug() << modelIndex.column();
                if (modelIndex.column() == 0)
                    selectedFilePaths.append(directory().absolutePath() + modelIndex.data().toString());
            }
            emit filesSelected(selectedFilePaths);
            hide();
            qDebug() << selectedFilePaths;
       }

    private:
        QTreeView *treeView;
        QPushButton *openButton;
        QStringList selectedFilePaths;
};
*/
