/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QStringList>

// QtGUI includes
#include "qSlicerDirectoryListView.h"

// --------------------------------------------------------------------------
// qSlicerDirectoryListViewPrivate

//-----------------------------------------------------------------------------
class qSlicerDirectoryListViewPrivate
{
  Q_DECLARE_PUBLIC(qSlicerDirectoryListView);
protected:
  qSlicerDirectoryListView* const q_ptr;

public:
  qSlicerDirectoryListViewPrivate(qSlicerDirectoryListView& object);
  void init();

  void addDirectory(const QString& path);

  enum
    {
    AbsolutePathRole = Qt::UserRole + 1
    };

  QListView*         ListView;
  QStandardItemModel DirectoryListModel;
};

// --------------------------------------------------------------------------
// qSlicerDirectoryListViewPrivate methods

// --------------------------------------------------------------------------
qSlicerDirectoryListViewPrivate::qSlicerDirectoryListViewPrivate(qSlicerDirectoryListView& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListViewPrivate::init()
{
  Q_Q(qSlicerDirectoryListView);

  this->ListView = new QListView();
  this->ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->ListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->ListView);
  q->setLayout(layout);

  this->ListView->setModel(&this->DirectoryListModel);
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListViewPrivate::addDirectory(const QString& path)
{
  Q_Q(qSlicerDirectoryListView);
  QStandardItem * item = new QStandardItem(path);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  if (!QFile::exists(absolutePath) || q->hasDirectory(absolutePath))
    {
    return;
    }
 // this->DirectoryListModel.removeRows(0, (this->DirectoryListModel.rowCount()));//rajouter
 // this->DirectoryListModel.removeRow(selectedIndexes.at(0).row());
  qDebug() << "directorylist [" << q->directoryList() << "]";
  qDebug() << "q->disabledDirectoryList() [" << q->disabledDirectoryList() << "]";
  qDebug() << "wherepath [" << qPrintable(path) << "]";
    //this->DirectoryListModel.removeRows(0, (this->DirectoryListModel.rowCount()));//rajouter
    item->setData(QVariant(Qt::darkBlue), Qt::ForegroundRole);
    item->setData(QVariant(absolutePath), Qt::ToolTipRole);//juste ces 3
    item->setData(QVariant(absolutePath), qSlicerDirectoryListViewPrivate::AbsolutePathRole);//juste ces 3
    this->DirectoryListModel.appendRow(item);//juste ces 3
    qDebug() << "directorylist [" << q->directoryList() << "]";
    qDebug() << "disabledDirectoryList() [" << q->disabledDirectoryList() << "]";

}

// --------------------------------------------------------------------------
// qSlicerDirectoryListView methods

// --------------------------------------------------------------------------
qSlicerDirectoryListView::qSlicerDirectoryListView(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerDirectoryListViewPrivate(*this))
{
  Q_D(qSlicerDirectoryListView);
  d->init();
}

// --------------------------------------------------------------------------
qSlicerDirectoryListView::~qSlicerDirectoryListView()
{
}

// --------------------------------------------------------------------------
QStringList qSlicerDirectoryListView::directoryList(bool absolutePath)const
{
  Q_D(const qSlicerDirectoryListView);
  QStringList directoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = qSlicerDirectoryListViewPrivate::AbsolutePathRole;
    }
  for(int i = 0; i < d->DirectoryListModel.rowCount(); ++i)
    {
    directoryList << d->DirectoryListModel.data(d->DirectoryListModel.index(i, 0), role).toString();
    }
  return directoryList;
}

// --------------------------------------------------------------------------
QStringList qSlicerDirectoryListView::selectedDirectoryList(bool absolutePath)const
{
  Q_D(const qSlicerDirectoryListView);
  QStringList directoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = qSlicerDirectoryListViewPrivate::AbsolutePathRole;
    }
  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  foreach(const QModelIndex& index, selectedIndexes)
    {
    directoryList << d->DirectoryListModel.data(index, role).toString();
    }
  return directoryList;
}

// --------------------------------------------------------------------------
bool qSlicerDirectoryListView::hasDirectory(const QString& path)const
{
  Q_D(const qSlicerDirectoryListView);
  QString absolutePath = QFileInfo(path).absoluteFilePath();
  QModelIndexList foundIndexes = d->DirectoryListModel.match(
        d->DirectoryListModel.index(0, 0), qSlicerDirectoryListViewPrivate::AbsolutePathRole,
        QVariant(absolutePath));
  Q_ASSERT(foundIndexes.size() < 2);
  return (foundIndexes.size() != 0);
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::addDirectory(const QString& path)
{
 // qDebug() << "addDirectory" << path;
  Q_D(qSlicerDirectoryListView);
  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  QStandardItem* item = new QStandardItem(path);

  if (false  == this->directoryList().contains(path) && false  == this->disabledDirectoryList().contains(path) )
    {
    d->addDirectory(path);
    emit this->directoryListChanged();
    return;
    }
  if (this->directoryList().contains(path) ) //est ce que la selection appartient a directorylist
    {
    d->addDirectory(path);
    item->setData(QVariant(Qt::red), Qt::ForegroundRole);
    d->DirectoryListModel.appendRow(item);
    this->directoryList().removeDuplicates();
    this->directoryList().removeOne(path);
    this->disabledDirectoryList().append(path);
    this->disabledDirectoryList().removeDuplicates();
    emit this->directoryListChanged();
    emit this->disabledDirectoryListChanged();
    return;
    }
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::removeDirectory(const QString& path)
{
  Q_D(qSlicerDirectoryListView);
  QList<QStandardItem*> foundItems = d->DirectoryListModel.findItems(path);
  Q_ASSERT(foundItems.count() < 2);
  if (foundItems.count() == 1)
    {
    d->DirectoryListModel.removeRow(foundItems.at(0)->row());
    emit this->directoryListChanged();
    }
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::removeSelectedDirectories()
{
  Q_D(qSlicerDirectoryListView);

  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  bool selectedCount = selectedIndexes.count();
  while(selectedIndexes.count() > 0)
    {
    d->DirectoryListModel.removeRow(selectedIndexes.at(0).row());
    selectedIndexes = d->ListView->selectionModel()->selectedRows();
    }
  if (selectedCount)
    {
    emit this->directoryListChanged();
    }
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::selectAllDirectories()
{
  Q_D(qSlicerDirectoryListView);
  d->ListView->selectAll();
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::clearDirectorySelection()
{
  Q_D(qSlicerDirectoryListView);
  d->ListView->clearSelection();
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::setDirectoryList(const QStringList& paths)
{
  Q_D(qSlicerDirectoryListView);
  qDebug() << "setDirectoryList-> selected directory list" << paths;
  if (paths.count() == this->directoryList().count())
    {
    int found = 0;
    foreach(const QString& path, paths)
      {
      if (this->hasDirectory(path))
        {
        ++found;
        }
      }
    if (found == paths.count())
      {
       qDebug() << "return setDirectory";
      return;
      }
    }

  d->DirectoryListModel.removeRows(0, d->DirectoryListModel.rowCount());

  foreach(const QString& path, paths)
    {
    d->addDirectory(path);
    }
  emit this->directoryListChanged();

}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::setDisabledDirectoryList(const QStringList& paths)// todo
{
  Q_D(qSlicerDirectoryListView);
  qDebug() << "paths:" << paths;
  qDebug() << "paths.count:" << paths.count();
  qDebug() << "this->disabledirectoryList():" << this->disabledDirectoryList();
  qDebug() << "this->disabledirectoryList().count:" << this->disabledDirectoryList().count();//? pourquoi il y a qq chose et dans setting la liste est vide
  if (paths.size() == this->disabledDirectoryList().size())
    {
    int found = 0;
    foreach(const QString& path, paths)
      {
      if (this->hasDirectory(path))
        {
        ++found;
        }
      }
    if (found == paths.count())
      {
       qDebug() << "return disabled directory list";
      return;
      }
    }

  d->DirectoryListModel.removeRows(0, d->DirectoryListModel.rowCount());

  foreach(const QString& path, paths)
    {
    d->addDirectory(path);
    }

  emit this->disabledDirectoryListChanged();

}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::enableSelectedDirectories()
{
  Q_D(qSlicerDirectoryListView);
  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows(0);
  if ( selectedIndexes.count() >0 )
    {
    this->selectedDirectoryList().removeDuplicates();
    //d->DirectoryListModel.removeRows(0, this->directoryList().count());
    foreach(const QString& path, this->selectedDirectoryList())
          {

          QStandardItem* item = new QStandardItem(path);
          //QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows(0);
          qDebug() << "q->directoryList().isEmpty()" << this->directoryList().isEmpty();
  qDebug() << "q->disabledDirectoryList().isEmpty()" << this->disabledDirectoryList().isEmpty();
  qDebug() << "q->directoryList().contains(path, Qt::CaseInsensitive)" << this->directoryList().contains(path, Qt::CaseInsensitive);
  qDebug() << "q->disabledDirectoryList().contains(path, Qt::CaseInsensitive)" << this->disabledDirectoryList().contains(path, Qt::CaseInsensitive);
     if (this->directoryList().contains(path)) //est ce que la selection appartient a directorylist
       {
       this->addDirectory(path);
       /*item->setData(QVariant(Qt::red), Qt::ForegroundRole);
       d->DirectoryListModel.appendRow(item);
       this->directoryList().removeOne(path);
       qDebug() << "this->directoryList().removeOne(path) [" << this->directoryList().removeOne(path) << "]";
       this->directoryList().removeDuplicates();
       qDebug() << "this->directoryList().removeDuplicates() [" << this->directoryList().removeDuplicates() << "]";
       //this->disabledDirectoryList().append(path);

       qDebug() << "this->disabledDirectoryList().append(path) [" << this->disabledDirectoryList() << "]";
       this->disabledDirectoryList().removeDuplicates();
       qDebug() << "this->disabledDirectoryList().removeDuplicates() [" << this->disabledDirectoryList().removeDuplicates() << "]";*/
       emit this->directoryListChanged();
       emit this->disabledDirectoryListChanged();
       qDebug() << "directorylist [" << this->directoryList() << "]";
    qDebug() << "q->disabledDirectoryList() [" << this->disabledDirectoryList() << "]";

       return;
      }
    if (this->disabledDirectoryList().contains(path, Qt::CaseInsensitive) )
      {
      //d->addDirectory(path); //private
      item->setData(QVariant(Qt::darkYellow), Qt::ForegroundRole);
      d->DirectoryListModel.appendRow(item);
      this->disabledDirectoryList().removeOne(path);
      this->disabledDirectoryList().removeDuplicates();
      this->directoryList().append(path);
      emit this->disabledDirectoryListChanged();
      this->directoryList().removeDuplicates();
      emit this->directoryListChanged();
      return;
      }
    /*else
        {
        this->setDirectoryList(this->selectedDisabledDirectoryList());
      //this->selectedDisabledDirectoryList();
        emit this->disabledDirectoryListChanged();
       // emit this->directoryListChanged();
        } */
     // this->setDisabledDirectoryList(this->selectedDirectoryList());
        //emit this->disabledDirectoryListChanged();
        //this->addDirectory(this->selectedDirectoryList()[i]);
        qDebug() << "this->DirectoryList():" << this->directoryList();
      //  this->selectedDirectoryList();
       // emit this->disabledDirectoryListChanged();
        //emit this->directoryListChanged();

          }
      }
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::removeDisabledSelectedDirectories()
{
  Q_D(qSlicerDirectoryListView);

  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  bool selectedCount = selectedIndexes.count();
  while(selectedIndexes.count() > 0)
    {
    d->DirectoryListModel.removeRow(selectedIndexes.at(0).row());
    selectedIndexes = d->ListView->selectionModel()->selectedRows();
    }
  if (selectedCount)
    {
    emit this->disabledDirectoryListChanged();
    }
}

// --------------------------------------------------------------------------
QStringList qSlicerDirectoryListView::selectedDisabledDirectoryList(bool absolutePath)const
{
  Q_D(const qSlicerDirectoryListView);
  QStringList disabledDirectoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = qSlicerDirectoryListViewPrivate::AbsolutePathRole;
    }
  QModelIndexList selectedIndexes = d->ListView->selectionModel()->selectedRows();
  foreach(const QModelIndex& index, selectedIndexes)
    {
    disabledDirectoryList << d->DirectoryListModel.data(index, role).toString();
    }
  return disabledDirectoryList;
}

 // --------------------------------------------------------------------------
QStringList qSlicerDirectoryListView::disabledDirectoryList(bool absolutePath)const
{
  Q_D(const qSlicerDirectoryListView);
  QStringList disabledDirectoryList;
  int role = Qt::DisplayRole;
  if (absolutePath)
    {
    role = qSlicerDirectoryListViewPrivate::AbsolutePathRole;
    }
  for(int i = 0; i < d->DirectoryListModel.rowCount(); ++i)
    {
    disabledDirectoryList << d->DirectoryListModel.data(d->DirectoryListModel.index(i, 0), role).toString();
    }
  return disabledDirectoryList;
}

// --------------------------------------------------------------------------
void qSlicerDirectoryListView::removeDisabledDirectory(const QString& path)
{
  Q_D(qSlicerDirectoryListView);
  QList<QStandardItem*> foundItems = d->DirectoryListModel.findItems(path);
  Q_ASSERT(foundItems.count() < 2);
  if (foundItems.count() == 1)
    {
    d->DirectoryListModel.removeRow(foundItems.at(0)->row());
    emit this->disabledDirectoryListChanged();
    }
}