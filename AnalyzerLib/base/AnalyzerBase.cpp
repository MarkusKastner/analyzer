/* Copyright (C) 2016-2017 - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Markus Kastner <markus.kastner@marscode.at>
*/

#include "AnalyzerBase.h"

#include <fstream>
#include <filesystem>

#include "AnalyzerLib\interpreter\Interpreter.h"
#include "AnalyzerLib\base\error\AnalyzerBaseException.h"
#include "AnalyzerBaseObserver.h"
#include "AnalyzerLib\core\zip\ZIPContainer.h"

namespace fs = std::tr2::sys;

namespace analyzer{
  namespace base{
    AnalyzerBase::AnalyzerBase()
      : baseThread(nullptr), runBaseWorker(true),
      workCondition(), waitLock(),
      workerException(),
      workTasks(),
      documentPath(),
      baseObservers(),
      workTasksLock(),
      files(), filesLock(),
      applicationSettings(), 
      checkerVault()
    {
      this->baseThread = new std::thread(&AnalyzerBase::baseWorker, this);
    }

    AnalyzerBase::~AnalyzerBase()
    {
      this->runBaseWorker = false;

      if (this->baseThread != nullptr){
        this->workCondition.notify_one();
        this->baseThread->join();
        delete this->baseThread;
      }
    }

    bool AnalyzerBase::HasActivefile()
    {
      if (!this->documentPath.empty() && !this->files.empty()){
        for (auto& file : this->files){
          if (file->GetFileName().compare(this->documentPath) == 0){
            return true;
          }
        }
      }
      return false;
    }

    core::File * AnalyzerBase::CurrentFile()
    {
      for (auto& file : this->files){
        if (file->GetFileName().compare(this->documentPath) == 0){
          return file.get();
        }
      }
      return nullptr;
    }

    size_t AnalyzerBase::NumberOfObservers()
    {
      return this->baseObservers.size();
    }

    void AnalyzerBase::RegisterObserver(AnalyzerBaseObserver * observer)
    {
      this->throwInvalidObserver(observer);
      for (auto it = this->baseObservers.begin(); it != this->baseObservers.end(); ++it){
        if ((*it) == observer){
          return;
        }
      }
      this->baseObservers.push_back(observer);
    }

    void AnalyzerBase::UnregisterObserver(AnalyzerBaseObserver * observer)
    {
      this->throwInvalidObserver(observer);
      for (auto it = this->baseObservers.begin(); it != this->baseObservers.end(); ++it){
        if ((*it) == observer){
          this->baseObservers.erase(it);
          return;
        }
      }
    }

    void AnalyzerBase::OpenDocument(const std::string & path)
    {
      fs::path dir(path);
      dir.remove_filename();
      this->applicationSettings.SetLastOpenDir(dir.generic_string());
      this->applicationSettings.Serialize();

      this->documentPath = path;
      this->addTask(Task::LoadNewDataFromFile);
      this->workCondition.notify_one();
    }

    void AnalyzerBase::CloseDocument()
    {
      this->documentPath = "";
      this->files.clear();
      this->notifyFilesChange();
    }

    bool AnalyzerBase::HasData()
    {
      return !this->files.empty();
    }

    void AnalyzerBase::Rethrow()
    {
      if (this->workerException){
        std::rethrow_exception(this->workerException);
      }
    }

    void AnalyzerBase::AddAnalyzerFile(const std::shared_ptr<core::File> & file)
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (auto& exisiting : this->files){
        if (exisiting->GetFileName().compare(file->GetFileName()) == 0){
          return;
        }
      }
      file->RegisterFileObserver(this);
      this->files.push_back(file);
      if (this->files.size() == 1){
        this->documentPath = file->GetFileName();
      }
      
      if (file->GetInterpreter()) {
        file->GetInterpreter()->DoSpecialProcessing();
      }
    }

    bool AnalyzerBase::HasFiles()
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      return !this->files.empty();
    }

    bool AnalyzerBase::HasFile(const std::string & fileName)
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (auto& exisiting : this->files){
        if (exisiting->GetFileName().compare(fileName) == 0){
          return true;
        }
      }
      return false;
    }

    size_t AnalyzerBase::FileCount()
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      return this->files.size();
    }

    std::shared_ptr<core::File> AnalyzerBase::GetAnalyzerFile(const std::string & fileName)
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (auto& exisiting : this->files){
        if (exisiting->GetFileName().compare(fileName) == 0){
          return exisiting;
        }
      }
      throw AnalyzerBaseException("unknown file");
    }

    std::shared_ptr<core::File> AnalyzerBase::GetAnalyzerFile(const size_t & index)
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (size_t i = 0; i < this->files.size(); i++){
        if (index == i){
          return this->files.at(i);
        }
      }
      throw AnalyzerBaseException("invalid index");
    }

    core::File * AnalyzerBase::GetActiveAnalyzerFile()
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (auto& exisiting : this->files){
        if (exisiting->GetFileName().compare(this->documentPath) == 0){
          return exisiting.get();
        }
      }
      return nullptr;
    }

    std::vector<std::string> AnalyzerBase::GetFileNames()
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      std::vector<std::string> fileNames;
      for (auto& file : this->files){
        fileNames.push_back(file->GetFileName());
      }
      return fileNames;
    }

    void AnalyzerBase::SetActiveFile(const std::string & fileName)
    {
      std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      for (auto& exisiting : this->files){
        if (exisiting->GetFileName().compare(fileName) == 0){
          this->documentPath = fileName;
          this->checkerVault.SetCurrentData(exisiting->GetData());
          this->activeFileChanged();
          return;
        }
      }
      throw AnalyzerBaseException("unknown file");
    }

    void AnalyzerBase::AddInternalFile(const std::shared_ptr<analyzer::core::File>& file)
    {
      {std::lock_guard<std::recursive_mutex> lock(this->filesLock);
      file->RegisterFileObserver(this);
      this->files.push_back(file);
      }
    }

    void AnalyzerBase::SetApplicationDirectory(const std::string & appDir)
    {
      this->applicationSettings.SetAppDir(appDir);
      if (this->applicationSettings.SettingsFileExists()) {
        this->applicationSettings.Deserialize();
      }
      else {
        this->applicationSettings.Serialize();
      }
    }

    std::string AnalyzerBase::GetLastOpenDir()
    {
      return this->applicationSettings.GetLastOpenDir();
    }

    void AnalyzerBase::SetLastOpenDir(const std::string & lastOpenDir)
    {
      this->applicationSettings.SetLastOpenDir(lastOpenDir);
      this->applicationSettings.Serialize();
    }

    checker::ContentCheckerVault & AnalyzerBase::GetContentCheckerVault()
    {
      return this->checkerVault;
    }

    void AnalyzerBase::baseWorker()
    {
      std::unique_lock<std::mutex> lock(this->waitLock);
      while (this->runBaseWorker.load()){
        try{
          this->workCondition.wait(lock);
          while (this->hasTask()){
            
            switch (this->getNextTask()){
            case Task::LoadNewDataFromFile:
              this->loadFile();
              break;
            }
          }
        }
        catch (...){
          this->workerException = std::current_exception();
          break;
        }
      }
    }

    bool AnalyzerBase::hasTask()
    {
      std::lock_guard<std::recursive_mutex> lock(this->workTasksLock);
      return !this->workTasks.empty();
    }

    AnalyzerBase::Task AnalyzerBase::getNextTask()
    {
      std::lock_guard<std::recursive_mutex> lock(this->workTasksLock);
      Task nextTask = this->workTasks.front();
      this->workTasks.pop();
      return nextTask;
    }

    void AnalyzerBase::addTask(const AnalyzerBase::Task & task)
    {
      std::lock_guard<std::recursive_mutex> lock(this->workTasksLock);
      this->workTasks.push(task);
    }

    void AnalyzerBase::loadFile()
    {
      std::string fileType;
      size_t p = this->documentPath.find_last_of('.');
      if (p != 0){
        size_t endingLen = this->documentPath.length() - p;
        fileType = this->documentPath.substr(p + 1, endingLen);
      }

      if (fileType.compare("zip") == 0){
        this->loadContainer();
      }
      else if (fileType.compare("docx") == 0){
        this->loadContainer();
      }
      else{
        this->loadSimpleFile();
      }
      this->notifyFilesChange();
    }

    void AnalyzerBase::loadSimpleFile()
    {
      std::ifstream file(this->documentPath.c_str(), std::ios::binary);
      if (file.bad() || !file.is_open()){
        throw AnalyzerBaseException("Cannot open " + this->documentPath);
      }

      std::vector<unsigned char> data;

      size_t fileSize = 0;
      file.seekg(0, std::ios::end);
      fileSize = static_cast<size_t>(file.tellg());
      file.seekg(0, std::ios::beg);

      data.reserve(fileSize);
      data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      fs::path path(this->documentPath);
      std::wstring fileNameW(path.filename().c_str());
      std::string fileName(fileNameW.begin(), fileNameW.end());
      this->AddAnalyzerFile(std::shared_ptr<core::File>(new core::PrimaryFile(fileName, data)));
    }

    void AnalyzerBase::loadContainer()
    {
      core::ZIPContainer zip;
      zip.Open(this->documentPath);
      if (zip.HasContent()){
        for (size_t i = 0; i < zip.GetFileCount(); i++){
          this->AddAnalyzerFile(zip.GetFileAt(i));
        }
      }
    }

    void AnalyzerBase::notifyFilesChange()
    {
      for (auto observer : this->baseObservers){
        observer->NotifyDocumentChange();
      }
    }

    void AnalyzerBase::activeFileChanged()
    {
      for (auto observer : this->baseObservers) {
        observer->FileChange();
      }
    }

    void AnalyzerBase::throwInvalidObserver(AnalyzerBaseObserver * observer)
    {
      if (nullptr == observer){
        throw AnalyzerBaseException("Invalid base observer");
      }
    }
  }
}