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

#ifndef __qSlicerDirectoryListView_h
#define __qSlicerDirectoryListView_h

// Qt includes
#include <QWidget>

// QtGUI includes
#include "qSlicerBaseQTGUIExport.h"
#include "qSlicerCoreApplication.h"

class qSlicerDirectoryListViewPrivate;

class Q_SLICER_BASE_QTGUI_EXPORT qSlicerDirectoryListView : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QStringList enabledDirectoryList READ enabledDirectoryList WRITE setEnabledDirectoryList NOTIFY enabledDirectoryListChanged);
  Q_PROPERTY(QStringList disabledDirectoryList READ disabledDirectoryList WRITE setDisabledDirectoryList NOTIFY disabledDirectoryListChanged);
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit qSlicerDirectoryListView(QWidget* parent = 0);

  /// Destructor
  virtual ~qSlicerDirectoryListView();

  /// Return True if the \a path has already been added
  bool hasDirectory(const QString& path)const;

  bool isDirectoryEnabled(const QString& path)const;

  void setDirectoryEnabled(const QString& path, bool enabled);

  QStringList enabledDirectoryList(bool absolutePath = false)const;
  QStringList disabledDirectoryList(bool absolutePath = false)const;

  QStringList selectedDirectoryList(bool absolutePath = false)const;
  QStringList selectedDisabledDirectoryList(bool absolutePath = false)const;

public slots:

  /// If \a path exists, add it to the view and emit signal directoryListChanged().
  /// \sa directoryListChanged()
  void addDirectory(const QString& path, bool enabled = true);

  /// Remove all entries and set \a paths has current list.
  /// The signal directoryListChanged() is emitted if the current list of directories is
  /// different from the provided one.
  /// \sa addDirectory(), directoryListChanged()
  void setEnabledDirectoryList(const QStringList& paths);

  void setDisabledDirectoryList(const QStringList& paths);

  /// Remove \a path from the list.
  /// The signal directoryListChanged() is emitted if the path was in the list.
  /// \sa directoryListChanged()
  void removeDirectory(const QString& path);

  void removeDisabledDirectory(const QString& path);

  /// \sa selectAllDirectories()
  void removeSelectedDirectories();

  void removeDisabledSelectedDirectories();//const QStringList& paths

  /// Select all directories.
  void selectAllDirectories();

  /// Clear the current directory selection.
  void clearDirectorySelection();

  /// Enable the selected directories.
  void enableSelectedDirectories();

signals:
  /// This signal is emitted when a directory is added to the view.
  void directoryListChanged();

protected:
  QScopedPointer<qSlicerDirectoryListViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerDirectoryListView);
  Q_DISABLE_COPY(qSlicerDirectoryListView);
  
};

#endif
