#ifndef ActTaskManager_h
#define ActTaskManager_h

#include "ActVTask.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>


namespace ActAlgorithm
{
class TaskManager
{
public:
    using TaskFcn = std::function<bool()>;
    using TaskPtr = std::shared_ptr<VTask>;

private:
    std::vector<std::string> fTaskIDs {};
    std::vector<TaskFcn> fTasks {};
    // Store plugin VTasks; other tasks may be owned by other classes (like MergerDet) so we are not responsible for
    // them
    std::vector<TaskPtr> fPlugins {};

public:
    TaskManager() = default;

    // Adding tasks
    // Add task by lambda function. Intended for fcns already defined in MergerDet
    void AddTask(const std::string& taskID, TaskFcn task);
    // Add plugin tasks
    void AddTask(const std::string& path, const std::string& taskID, const std::string& at);

    // Getters
    std::vector<TaskPtr> GetPlugins() { return fPlugins; }
    std::vector<TaskFcn> GetTasks() { return fTasks; }

    // Other functions
    void Print() const;

private:
    void LoadPlugin(const std::string& path, const std::string& taskID);
    void AddAt(TaskPtr ptr, const std::string& at);
};
} // namespace ActAlgorithm

#endif // !ActTaskManager_h
