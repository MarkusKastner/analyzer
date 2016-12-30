#include "DocumentStructure.h"

#include <QApplication>
#include <QVBoxLayout>

#include <regex>

namespace analyzer{
  namespace gui{
    DocumentStructure::DocumentStructure(const QString & title, QWidget * parent)
      :QDockWidget(title, parent), baseWidget(nullptr), fileTree(nullptr)
    {
      this->setup();
    }

    DocumentStructure::~DocumentStructure()
    {

    }

    void DocumentStructure::SetFiles(const std::vector<std::string> files)
    {
      QApplication::postEvent(this, new FilesEvent(files));
    }

    void DocumentStructure::customEvent(QEvent * evt)
    {
      if (dynamic_cast<FilesEvent*>(evt)){
        this->setFiles(dynamic_cast<FilesEvent*>(evt)->GetFiles());
      }
    }

    void DocumentStructure::setup()
    {
      this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      this->baseWidget = new QWidget();
      this->setWidget(this->baseWidget);
      this->baseWidget->setLayout(new QVBoxLayout());

      this->fileTree = new QTreeWidget(this->baseWidget);
      this->baseWidget->layout()->addWidget(this->fileTree);
      this->fileTree->setHeaderHidden(true);
      connect(this->fileTree, &QTreeWidget::itemClicked, this, &DocumentStructure::onItemChanged);
    }

    void DocumentStructure::setFiles(const std::vector<std::string> files)
    {
      this->fileTree->clear();

      for (auto& file : files){
        QTreeWidgetItem * parent = this->checkDir(file);
        QTreeWidgetItem * item = createFileItem(file);
        if (parent == nullptr){
          this->addFile(item);
        }
        else{
          this->addFile(parent, item);
        }
      }
    }

    QTreeWidgetItem * DocumentStructure::checkDir(const std::string & file)
    {
      std::vector<std::string> dir(getSplittedDir(file));
      if (dir.empty()){
        return nullptr;
      }
      QTreeWidgetItem * parentItem = nullptr;
      for (auto& folder : dir){
        QList<QTreeWidgetItem *> entries;
        if (parentItem == nullptr){
          entries = this->fileTree->findItems(folder.c_str(), Qt::MatchFixedString);
        }
        else{
          for (int i = 0; i < parentItem->childCount(); i++){
            if (parentItem->child(i)->text(0).compare(QString(folder.c_str())) == 0){
              entries.push_back(parentItem->child(i));
            }
          }
        }

        if (entries.empty()){
          parentItem = new QTreeWidgetItem(parentItem);
          parentItem->setText(0, folder.c_str());
          this->fileTree->addTopLevelItem(parentItem);
        }
        else{
          parentItem = entries.at(0);
        }
      }
      return parentItem;
    }

    QTreeWidgetItem * DocumentStructure::createFileItem(const std::string & file)
    {
      FileItem * item = new FileItem();
      item->setText(0, this->getFileName(file).c_str());
      item->SetFilePath(file);
      return item;
    }

    void DocumentStructure::addFile(QTreeWidgetItem * item)
    {
      this->fileTree->addTopLevelItem(item);
    }

    void DocumentStructure::addFile(QTreeWidgetItem * parent, QTreeWidgetItem * item)
    {
      parent->addChild(item);
    }

    std::vector<std::string> getPathParts(const std::string & fullFileName){
      std::regex re("/");
      std::sregex_token_iterator first{ fullFileName.begin(), fullFileName.end(), re, -1 }, last;
      std::vector<std::string> parts{ first, last };
      return parts;
    }

    std::vector<std::string> DocumentStructure::getSplittedDir(const std::string & dir)
    {
      std::vector<std::string> parts(getPathParts(dir));
      if (parts.size() > 0){
        parts.erase(parts.begin() + parts.size() - 1);
      }
      return parts;
    }

    std::string DocumentStructure::getFileName(const std::string & file)
    {
      std::vector<std::string> parts(getPathParts(file));
      if (parts.empty()){
        return "";
      }
      else{
        return parts.back();
      }
    }

    void DocumentStructure::onItemChanged(QTreeWidgetItem * item, int col)
    {
      auto fileItem = dynamic_cast<FileItem*>(item);
      if (fileItem != nullptr){
        this->ActiveFileChanged(fileItem->GetFilePath());
      }
    }
  }
}