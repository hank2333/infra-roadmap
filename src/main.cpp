#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

struct WeekSpec {
    int stage;
    std::string stage_name;
    std::string topic;
    std::string learn;
    std::string practice;
    std::string deliverable;
};

struct Task {
    std::string date;
    int stage{};
    std::string stage_name;
    int week{};
    std::string topic;
    int number{};
    int minutes{};
    std::string text;
};

struct DayPlan {
    std::string date;
    int stage{};
    std::string stage_name;
    int week{};
    std::string topic;
    std::vector<Task> tasks;
};

static const std::string kStartDate = "2026-08-05";
static const std::string kPlanFileName = "roadmap.tsv";

std::vector<WeekSpec> curriculum() {
    return {
        {0, "系统工程底座", "基线、工具链与工程习惯", "建立 CMake/Git/调试器/计时工具工作流", "为小程序加入日志、断言和基准入口", "提交环境清单、基线测试与一页学习地图"},
        {0, "系统工程底座", "进程、线程与系统调用", "梳理 fork/exec/wait、线程和上下文切换", "实现父子进程管道通信并处理错误码", "完成 process-lab，画出进程与线程关系图"},
        {0, "系统工程底座", "虚拟内存、mmap 与 COW", "理解页表、TLB、缺页、mmap 和写时复制", "编写 mmap 文件读写与缺页观察实验", "输出 vm-lab、测量结果和内存路径图"},
        {0, "系统工程底座", "C++ 对象生命周期", "复习 RAII、移动语义、拷贝消除和所有权", "实现带移动语义的资源句柄并写单元测试", "完成 resource-handle 与生命周期说明"},
        {0, "系统工程底座", "分配器、内存池与局部性", "理解 allocator、arena、对齐和碎片", "实现定长对象池并与 new/delete 对比", "提交 memory-pool、基准表与设计取舍"},
        {0, "系统工程底座", "并发同步基础", "掌握 mutex、condition_variable、future 和线程池", "实现有界阻塞队列与固定线程池", "完成 thread-pool、压力测试和停机协议"},
        {0, "系统工程底座", "原子与 C++ 内存模型", "理解 happens-before、memory_order 和伪共享", "实现 SPSC 队列并验证正确性", "提交 atomic-lab、缓存行对齐实验与笔记"},
        {0, "系统工程底座", "系统项目整合", "复盘错误处理、资源管理、并发与基准方法", "把日志、线程池、内存池接入 mini runtime", "发布 systems-foundation v0.1 与技术总结"},

        {1, "体系结构与性能", "CPU 缓存与数据布局", "理解缓存层级、cache line、预取和 AoS/SoA", "比较顺序/随机访问与 AoS/SoA 性能", "提交 cache-lab 与可复现实验报告"},
        {1, "体系结构与性能", "性能分析方法", "学习吞吐、延迟、火焰图和基准陷阱", "使用计时器和 profiler 定位一个热点", "产出 profiling-checklist 与优化前后数据"},
        {1, "体系结构与性能", "SIMD 与向量化", "理解 SIMD、AVX、对齐与编译器自动向量化", "实现标量与 SIMD 向量运算并检查汇编", "提交 simd-lab、加速比和误差分析"},
        {1, "体系结构与性能", "NUMA 与并行伸缩", "理解 NUMA、亲和性、带宽和伪共享", "做线程数伸缩与缓存行 padding 实验", "完成 scaling-report 与瓶颈解释"},
        {1, "体系结构与性能", "编译器优化与汇编", "比较 O0/O2/O3、内联、别名和循环优化", "用 Compiler Explorer/objdump 对照 C++ 与汇编", "提交 compiler-lab 和五个优化案例"},
        {1, "体系结构与性能", "CPU 性能阶段项目", "复盘 roofline、算术强度和性能证据链", "优化一版 CPU 矩阵运算并逐项测量", "发布 cpu-kernel v0.1 与性能报告"},

        {2, "GPU 与 CUDA", "GPU 执行模型与环境", "理解 SM、warp、block、grid 和 SIMT", "检查 CUDA 环境并运行最小 kernel；无 GPU 时用在线环境", "提交 device-info、执行模型图和环境记录"},
        {2, "GPU 与 CUDA", "向量加法与错误处理", "掌握 kernel launch、索引和同步", "实现 vector add，加入边界与 CUDA 错误检查", "完成 vector-add 与 CPU/GPU 正确性对照"},
        {2, "GPU 与 CUDA", "全局内存与合并访问", "理解 coalescing、带宽、占用率和 pinned memory", "比较连续、跨步和错位访问", "提交 memory-access-lab 与带宽表"},
        {2, "GPU 与 CUDA", "共享内存与归约", "掌握 shared memory、同步和 bank conflict", "实现多版本 reduction 并逐步优化", "完成 reduction、正确性测试和版本对比"},
        {2, "GPU 与 CUDA", "朴素矩阵乘", "推导 GEMM 索引、FLOPs 和边界处理", "实现 naive GEMM 并对照 CPU 结果", "提交 naive-gemm 与尺寸/误差测试"},
        {2, "GPU 与 CUDA", "分块矩阵乘", "理解 tiling、数据复用和 occupancy", "用 shared memory 优化 GEMM", "提交 tiled-gemm、加速比与瓶颈说明"},
        {2, "GPU 与 CUDA", "Softmax kernel", "理解数值稳定 softmax 与并行归约", "实现 row-wise softmax 并测试极值输入", "完成 softmax kernel、误差和吞吐报告"},
        {2, "GPU 与 CUDA", "CUDA 性能分析", "学习 kernel timeline、occupancy 和 roofline 思路", "用 Nsight 或替代工具分析现有 kernels", "提交 profile 截图、三项瓶颈和优化计划"},
        {2, "GPU 与 CUDA", "转置、融合与访存优化", "理解 transpose、bank conflict 与 kernel fusion", "实现 tiled transpose 或 add+activation 融合", "完成 fusion-lab 与内存流量估算"},
        {2, "GPU 与 CUDA", "CUDA 算子库阶段项目", "复盘 kernel API、测试、benchmark 和文档标准", "整合 add/matmul/softmax/reduction 为小型库", "发布 cuda-kernels v0.1 和统一 benchmark"},

        {3, "深度学习框架内部", "Tensor 布局与 stride", "理解 shape、stride、contiguous、view 和 broadcast", "用 C++ 实现 Tensor 元数据与索引", "提交 tensor-core 的布局测试和示意图"},
        {3, "深度学习框架内部", "Storage 与基础算子", "理解 storage、dtype、device 和所有权", "实现 add/mul/matmul 的 CPU 路径", "完成算子测试、广播规则与错误处理"},
        {3, "深度学习框架内部", "计算图与自动微分", "理解动态图、拓扑序和梯度累积", "实现 Value/Tensor 计算图节点", "提交 autograd graph 与可视化例子"},
        {3, "深度学习框架内部", "Backward 与优化器", "推导 add/mul/matmul/ReLU 的反向传播", "实现 backward、zero_grad 和 SGD", "训练一个小模型并通过数值梯度检查"},
        {3, "深度学习框架内部", "Python/C++ 绑定", "理解 pybind11、ABI、GIL 和数据所有权", "为 Tensor 核心封装 Python API", "实现 import mini_tensor 与基础调用演示"},
        {3, "深度学习框架内部", "PyTorch C++/CUDA Extension", "理解 ATen dispatch、extension 构建和 contiguous 约束", "把一个 CUDA 算子接入 PyTorch", "提交 extension、正确性测试和 benchmark"},
        {3, "深度学习框架内部", "Mini Tensor 阶段发布", "复盘 API、autograd、CPU/CUDA 路径和测试", "整理仓库、CI、示例与性能数据", "发布 mini-tensor v0.1 和架构文档"},

        {4, "LLM 推理系统", "Transformer 推理计算", "逐项推导 RMSNorm、RoPE、MLP 和 Attention", "用小张量手算并实现单层 forward", "输出 inference-math 笔记和可运行参考实现"},
        {4, "LLM 推理系统", "模型权重与 tokenizer", "理解 checkpoint、dtype、tokenizer 和权重布局", "加载小模型配置与权重元数据", "提交 model-inspector 与内存占用估算"},
        {4, "LLM 推理系统", "Prefill、Decode 与性能指标", "理解 TTFT、TPOT、吞吐和算术强度差异", "测量 Hugging Face 小模型 prefill/decode", "完成 latency-baseline 与实验脚本"},
        {4, "LLM 推理系统", "KV Cache 基础", "推导 KV cache 形状、容量和生命周期", "实现连续 KV cache 管理器", "提交 kv-cache v0.1 与显存计算器"},
        {4, "LLM 推理系统", "Attention 推理实现", "理解 MHA/GQA、causal mask 和数值稳定性", "实现带 KV cache 的 attention 参考版本", "完成 correctness cases 与性能基线"},
        {4, "LLM 推理系统", "Paged KV Cache", "理解分页、block table、碎片与共享前缀", "实现 CPU 模拟的 block allocator", "提交 paged-kv 模拟器与碎片实验"},
        {4, "LLM 推理系统", "请求队列与调度器", "理解 FCFS、token budget、抢占和公平性", "实现可测试的 request scheduler", "完成 scheduler v0.1 与策略对比"},
        {4, "LLM 推理系统", "Continuous Batching", "理解动态加入/退出、padding 浪费和迭代级调度", "实现 continuous batching 仿真", "提交吞吐/延迟权衡曲线与解释"},
        {4, "LLM 推理系统", "Serving 指标与压测", "理解并发、排队、P50/P95/P99 和 backpressure", "为 mini server 加入指标和负载生成器", "完成 serving benchmark 与瓶颈报告"},
        {4, "LLM 推理系统", "量化与推理优化", "理解 FP16/BF16/INT8/INT4、权重与 KV 量化", "对小模型做一种量化并比较质量/速度/内存", "提交 quantization-report 与选择依据"},
        {4, "LLM 推理系统", "Mini LLM Server 发布", "复盘 engine、KV cache、scheduler、batching 和 API", "整合 HTTP/CLI 请求到推理引擎并压测", "发布 mini-llm-server v0.1 与架构/性能文档"},

        {5, "分布式与生产化", "Collective 与分布式基础", "理解 broadcast、reduce、all-reduce、ring 和带宽模型", "用多进程模拟 collective；有 GPU 时补 NCCL 实验", "提交 collective-lab 与通信量推导"},
        {5, "分布式与生产化", "DP、TP、PP 与 ZeRO", "比较数据/张量/流水线并行和 ZeRO 分片", "为给定模型设计 2/4/8 GPU 切分方案", "完成 parallelism-design 与显存通信表"},
        {5, "分布式与生产化", "Docker 与 GPU 容器", "掌握镜像层、volume、network 和可复现构建", "为 mini server 编写 Dockerfile 与健康检查", "提交可构建镜像、运行说明和大小优化"},
        {5, "分布式与生产化", "Kubernetes 与 GPU 调度", "理解 Pod/Deployment/Service、资源限制和 GPU 调度", "编写 mini server 部署清单并本地校验", "提交 k8s manifests、扩缩容与故障说明"},
        {5, "分布式与生产化", "vLLM 源码定向阅读", "跟踪 request 到 scheduler、KV cache、worker 的调用链", "为关键类画调用图并运行一个最小调试案例", "产出 vLLM reading notes 与三个改进问题"},
        {5, "分布式与生产化", "TensorRT-LLM/DeepSpeed 定向阅读", "比较图优化、kernel fusion、ZeRO 和并行策略", "复现一个官方最小例或做源码路径追踪", "完成框架对比表与选型说明"},
        {5, "分布式与生产化", "简历、面试与投递冲刺", "整理系统/CUDA/LLM 常见题与项目证据", "打磨两个主项目、做模拟面试并建立投递表", "发布作品集终版并启动分批投递"}
    };
}

std::tm parseDate(const std::string& value) {
    std::tm tm{};
    std::istringstream in(value);
    in >> std::get_time(&tm, "%Y-%m-%d");
    if (in.fail()) {
        throw std::runtime_error("日期格式应为 YYYY-MM-DD");
    }
    tm.tm_hour = 12;
    tm.tm_isdst = -1;
    std::time_t normalized = std::mktime(&tm);
    if (normalized == -1) {
        throw std::runtime_error("无效日期: " + value);
    }
    std::tm check = *std::localtime(&normalized);
    std::ostringstream out;
    out << std::put_time(&check, "%Y-%m-%d");
    if (out.str() != value) {
        throw std::runtime_error("无效日期: " + value);
    }
    return check;
}

std::string formatDate(const std::tm& tm) {
    std::ostringstream out;
    out << std::put_time(&tm, "%Y-%m-%d");
    return out.str();
}

std::string addDays(const std::string& date, int days) {
    std::tm tm = parseDate(date);
    tm.tm_mday += days;
    tm.tm_isdst = -1;
    std::mktime(&tm);
    return formatDate(tm);
}

int daysBetween(const std::string& earlier, const std::string& later) {
    std::tm first = parseDate(earlier);
    std::tm second = parseDate(later);
    const std::time_t first_time = std::mktime(&first);
    const std::time_t second_time = std::mktime(&second);
    return static_cast<int>(std::difftime(second_time, first_time) / (60 * 60 * 24));
}

std::string todayString() {
    const auto now = std::chrono::system_clock::now();
    std::time_t value = std::chrono::system_clock::to_time_t(now);
    std::tm local = *std::localtime(&value);
    return formatDate(local);
}

std::vector<std::string> split(const std::string& value, char delimiter) {
    std::vector<std::string> result;
    std::istringstream in(value);
    std::string item;
    while (std::getline(in, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string cleanField(std::string value) {
    std::replace(value.begin(), value.end(), '\t', ' ');
    std::replace(value.begin(), value.end(), '\r', ' ');
    std::replace(value.begin(), value.end(), '\n', ' ');
    return value;
}

fs::path executablePath(const char* argv0) {
#ifdef _WIN32
    std::vector<wchar_t> buffer(32768);
    DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (size > 0 && size < buffer.size()) {
        return fs::path(std::wstring(buffer.data(), size));
    }
#endif
    return fs::absolute(fs::path(argv0));
}

fs::path defaultProjectDataPath(const fs::path& exe) {
    const fs::path exe_dir = exe.parent_path();
    if (exe_dir.filename() == "build") {
        return exe_dir.parent_path() / "data" / kPlanFileName;
    }
    return exe_dir / kPlanFileName;
}

fs::path findDataPath(const fs::path& exe) {
    if (const char* custom = std::getenv("INFRA_PLAN_DATA")) {
        return fs::path(custom);
    }
    const std::vector<fs::path> candidates = {
        exe.parent_path() / kPlanFileName,
        exe.parent_path().parent_path() / "data" / kPlanFileName,
        fs::current_path() / "data" / kPlanFileName
    };
    for (const auto& path : candidates) {
        if (fs::exists(path)) {
            return path;
        }
    }
    return defaultProjectDataPath(exe);
}

fs::path progressPath() {
    if (const char* custom = std::getenv("INFRA_PROGRESS_FILE")) {
        return fs::path(custom);
    }
#ifdef _WIN32
    if (const char* local = std::getenv("LOCALAPPDATA")) {
        return fs::path(local) / "infra-roadmap" / "progress.tsv";
    }
#endif
    return fs::current_path() / ".infra-roadmap-progress.tsv";
}

std::pair<std::string, std::string> dailyTasks(const WeekSpec& spec, int day) {
    switch (day) {
        case 0:
            return {"阅读/推导：" + spec.learn + "；写 5 条要点和 2 个疑问（40 分钟）",
                    "准备本周仓库/实验入口，记录可复现的初始状态（35 分钟）"};
        case 1:
            return {"用自己的话复述核心机制，并画一张数据流或调用关系图（35 分钟）",
                    "最小实验：" + spec.practice + "；先跑通正确版本（40 分钟）"};
        case 2:
            return {"继续实现：" + spec.practice + "；补齐边界条件与错误处理（55 分钟）",
                    "为今天的实现写至少 3 个正确性检查（30 分钟）"};
        case 3:
            return {"定位并解释一个失败案例；用调试器/日志保留证据（40 分钟）",
                    "阅读一段相关官方文档或源码，给实现补 5 条行内笔记（35 分钟）"};
        case 4:
            return {"做一次可重复 benchmark：固定环境、预热、重复测量（45 分钟）",
                    "根据数据只做一项优化，并记录优化前后结果（40 分钟）"};
        case 5:
            return {"整合本周成果：" + spec.deliverable + "（50 分钟）",
                    "补 README：目标、运行方法、结果、限制和下一步（35 分钟）"};
        default:
            return {"闭卷回答本周 5 个问题，重做最薄弱的一个小实验（35 分钟）",
                    "周复盘：检查交付物、提交代码，写下下周风险与计划（25 分钟）"};
    }
}

void generatePlan(const fs::path& path) {
    const auto specs = curriculum();
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("无法写入路线文件: " + path.string());
    }
    out << "date\tstage\tstage_name\tweek\ttopic\ttask_no\tminutes\ttask\n";
    for (std::size_t week = 0; week < specs.size(); ++week) {
        for (int day = 0; day < 7; ++day) {
            const std::string date = addDays(kStartDate, static_cast<int>(week) * 7 + day);
            const auto tasks = dailyTasks(specs[week], day);
            const int first_minutes = (day == 2 ? 55 : day == 4 ? 45 : day == 5 ? 50 : day == 6 ? 35 : day == 0 ? 40 : 35);
            const int second_minutes = (day == 2 ? 30 : day == 4 ? 40 : day == 5 ? 35 : day == 6 ? 25 : day == 0 ? 35 : 40);
            out << date << '\t' << specs[week].stage << '\t' << cleanField(specs[week].stage_name)
                << '\t' << (week + 1) << '\t' << cleanField(specs[week].topic)
                << "\t1\t" << first_minutes << '\t' << cleanField(tasks.first) << '\n';
            out << date << '\t' << specs[week].stage << '\t' << cleanField(specs[week].stage_name)
                << '\t' << (week + 1) << '\t' << cleanField(specs[week].topic)
                << "\t2\t" << second_minutes << '\t' << cleanField(tasks.second) << '\n';
        }
    }
}

std::vector<Task> loadTasks(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("找不到路线数据: " + path.string() + "\n请先运行 infra init");
    }
    std::vector<Task> tasks;
    std::string line;
    std::getline(in, line);
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;
        const auto fields = split(line, '\t');
        if (fields.size() != 8) {
            throw std::runtime_error("路线数据格式错误: " + line);
        }
        tasks.push_back({fields[0], std::stoi(fields[1]), fields[2], std::stoi(fields[3]),
                         fields[4], std::stoi(fields[5]), std::stoi(fields[6]), fields[7]});
    }
    return tasks;
}

std::set<std::string> loadCompleted(const fs::path& path) {
    std::set<std::string> completed;
    std::ifstream in(path, std::ios::binary);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) completed.insert(line);
    }
    return completed;
}

void saveCompleted(const fs::path& path, const std::set<std::string>& completed) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("无法保存进度: " + path.string());
    }
    for (const auto& key : completed) out << key << '\n';
}

std::string taskKey(const std::string& date, int number) {
    return date + "|" + std::to_string(number);
}

std::map<std::string, DayPlan> groupByDay(const std::vector<Task>& tasks) {
    std::map<std::string, DayPlan> days;
    for (const auto& task : tasks) {
        auto& day = days[task.date];
        day.date = task.date;
        day.stage = task.stage;
        day.stage_name = task.stage_name;
        day.week = task.week;
        day.topic = task.topic;
        day.tasks.push_back(task);
    }
    return days;
}

std::vector<const Task*> overdueTasks(const std::vector<Task>& all_tasks,
                                      const std::string& selected_date,
                                      const std::set<std::string>& completed) {
    std::vector<const Task*> overdue;
    for (const auto& task : all_tasks) {
        if (task.date >= selected_date) break;
        if (!completed.count(taskKey(task.date, task.number))) {
            overdue.push_back(&task);
        }
    }
    return overdue;
}

void printDay(const DayPlan& day, const std::vector<Task>& all_tasks,
              const std::set<std::string>& completed) {
    int total_minutes = 0;
    int done_count = 0;
    for (const auto& task : day.tasks) {
        total_minutes += task.minutes;
        if (completed.count(taskKey(task.date, task.number))) ++done_count;
    }
    std::cout << "\n" << day.date << "  |  第 " << day.week << " 周  |  Stage "
              << day.stage << " " << day.stage_name << "\n";
    std::cout << "主题：" << day.topic << "  |  计划 " << total_minutes << " 分钟"
              << "  |  " << done_count << "/" << day.tasks.size() << " 完成\n";
    const auto overdue = overdueTasks(all_tasks, day.date, completed);
    if (!overdue.empty()) {
        int overdue_minutes = 0;
        for (const Task* task : overdue) overdue_minutes += task->minutes;
        std::cout << "\n延期待办：" << overdue.size() << " 项，共 " << overdue_minutes
                  << " 分钟（建议先清欠账，再做今日任务）\n";
        for (const Task* task : overdue) {
            const int delay = daysBetween(task->date, day.date);
            std::cout << "|" << task->date << "#" << task->number
                      << "\t[!] 已延迟 " << delay << " 天（原定 " << task->date << "） "
                      << task->text << "\n";
        }
        std::cout << "完成延期项：infra done <原定日期> <任务编号>\n";
    }
    std::cout << "\n今日计划\n";
    for (const auto& task : day.tasks) {
        const bool done = completed.count(taskKey(task.date, task.number)) > 0;
        std::cout << "|" << task.number << ".\t[" << (done ? "X" : " ") << "] "
                  << task.text << "\n";
    }
}

void printHelp() {
    std::cout <<
        "AI Infra Roadmap\n\n"
        "Usage:\n"
        "  infra today                  查看今天任务（无参数时默认）\n"
        "  infra show YYYY-MM-DD        查看指定日期（例如 2026-08-05）\n"
        "  infra week [YYYY-MM-DD]      查看日期所在的 7 天计划\n"
        "  infra stages                 查看所有阶段进度\n"
        "  infra stage <0-5>            查看阶段周主题与进度\n"
        "  infra progress               查看总进度、时间进度和当前阶段\n"
        "  infra done [YYYY-MM-DD] <n>  勾选某日第 n 项；省略日期表示今天\n"
        "  infra undo [YYYY-MM-DD] <n>  取消勾选\n"
        "  infra init                   生成/重建完整路线数据\n"
        "  infra help                   显示帮助\n";
}

const DayPlan& requireDay(const std::map<std::string, DayPlan>& days, const std::string& date) {
    parseDate(date);
    const auto it = days.find(date);
    if (it == days.end()) {
        throw std::runtime_error("日期不在计划范围内: " + date + "（" +
                                 days.begin()->first + " 至 " + days.rbegin()->first + "）");
    }
    return it->second;
}

void showWeek(const std::map<std::string, DayPlan>& days, const std::string& date,
              const std::vector<Task>& all_tasks, const std::set<std::string>& completed) {
    const auto& selected = requireDay(days, date);
    const std::string week_start = addDays(kStartDate, (selected.week - 1) * 7);
    for (int i = 0; i < 7; ++i) {
        printDay(requireDay(days, addDays(week_start, i)), all_tasks, completed);
    }
}

void printStageSummary(const std::vector<Task>& tasks, const std::set<std::string>& completed) {
    struct Counts { std::string name; int done{}; int total{}; int minutes{}; };
    std::map<int, Counts> counts;
    for (const auto& task : tasks) {
        auto& c = counts[task.stage];
        c.name = task.stage_name;
        c.total++;
        c.minutes += task.minutes;
        if (completed.count(taskKey(task.date, task.number))) c.done++;
    }
    std::cout << "\n阶段进度\n";
    for (const auto& [stage, c] : counts) {
        const int percent = c.total ? (100 * c.done / c.total) : 0;
        std::cout << "Stage " << stage << "  " << c.name << "  "
                  << c.done << "/" << c.total << " (" << percent << "%)"
                  << "  计划 " << c.minutes / 60.0 << " 小时\n";
    }
}

void printStage(const std::vector<Task>& tasks, int target,
                const std::set<std::string>& completed) {
    std::map<int, std::pair<std::string, std::string>> weeks;
    int total = 0;
    int done = 0;
    std::string name;
    for (const auto& task : tasks) {
        if (task.stage != target) continue;
        name = task.stage_name;
        weeks[task.week] = {task.date, task.topic};
        total++;
        if (completed.count(taskKey(task.date, task.number))) done++;
    }
    if (total == 0) throw std::runtime_error("阶段编号应为 0-5");
    std::cout << "\nStage " << target << "  " << name << "  " << done << "/" << total
              << " (" << (100 * done / total) << "%)\n";
    for (const auto& [week, value] : weeks) {
        std::cout << "  第 " << week << " 周  " << value.first << "  " << value.second << "\n";
    }
}

void printProgress(const std::vector<Task>& tasks, const std::map<std::string, DayPlan>& days,
                   const std::set<std::string>& completed) {
    int done = 0;
    for (const auto& task : tasks) {
        if (completed.count(taskKey(task.date, task.number))) ++done;
    }
    const std::string today = todayString();
    int elapsed = 0;
    for (const auto& [date, day] : days) {
        (void)day;
        if (date <= today) ++elapsed;
    }
    const int task_percent = tasks.empty() ? 0 : 100 * done / static_cast<int>(tasks.size());
    const int time_percent = days.empty() ? 0 : std::min(100, 100 * elapsed / static_cast<int>(days.size()));
    std::cout << "\n总进度\n"
              << "任务完成：" << done << "/" << tasks.size() << " (" << task_percent << "%)\n"
              << "时间进度：" << elapsed << "/" << days.size() << " 天 (" << time_percent << "%)\n"
              << "计划范围：" << days.begin()->first << " 至 " << days.rbegin()->first << "\n";
    if (days.count(today)) {
        std::cout << "当前主题：第 " << days.at(today).week << " 周 · " << days.at(today).topic << "\n";
        const auto overdue = overdueTasks(tasks, today, completed);
        if (overdue.empty()) {
            std::cout << "延期任务：0 项\n";
        } else {
            const int oldest_delay = daysBetween(overdue.front()->date, today);
            std::cout << "延期任务：" << overdue.size() << " 项；最早原定 "
                      << overdue.front()->date << "，已延迟 " << oldest_delay << " 天\n";
        }
    } else if (today < days.begin()->first) {
        std::cout << "计划尚未开始。\n";
    } else {
        std::cout << "计划日期已结束，请集中投递与复盘项目。\n";
    }
    printStageSummary(tasks, completed);
}

int parseTaskNumber(const std::string& value) {
    try {
        std::size_t used = 0;
        int number = std::stoi(value, &used);
        if (used != value.size() || number < 1) throw std::runtime_error("");
        return number;
    } catch (...) {
        throw std::runtime_error("任务编号必须是正整数");
    }
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        const fs::path exe = executablePath(argv[0]);
        const std::string command = argc >= 2 ? argv[1] : "today";
        fs::path data_path = findDataPath(exe);

        if (command == "init") {
            data_path = defaultProjectDataPath(exe);
            generatePlan(data_path);
            const auto tasks = loadTasks(data_path);
            const auto days = groupByDay(tasks);
            std::cout << "路线数据已生成：" << data_path.string() << "\n"
                      << "共 " << days.size() << " 天、" << tasks.size() << " 项任务，"
                      << days.begin()->first << " 至 " << days.rbegin()->first << "。\n";
            return 0;
        }
        if (command == "help" || command == "-h" || command == "--help") {
            printHelp();
            return 0;
        }

        const auto tasks = loadTasks(data_path);
        const auto days = groupByDay(tasks);
        const fs::path state_path = progressPath();
        auto completed = loadCompleted(state_path);

        if (command == "today") {
            const std::string today = todayString();
            if (today < days.begin()->first) {
                std::cout << "计划尚未开始。第一天是 " << days.begin()->first
                          << "，距离开始还有 " << daysBetween(today, days.begin()->first)
                          << " 天。\n";
            } else {
                printDay(requireDay(days, today), tasks, completed);
            }
        } else if (command == "show" || command == "date") {
            if (argc < 3) throw std::runtime_error("请提供日期，例如 infra show 2026-08-05");
            printDay(requireDay(days, argv[2]), tasks, completed);
        } else if (command == "week") {
            const std::string selected = argc >= 3
                ? argv[2]
                : std::max(todayString(), days.begin()->first);
            showWeek(days, selected, tasks, completed);
        } else if (command == "stages") {
            printStageSummary(tasks, completed);
        } else if (command == "stage") {
            if (argc < 3) throw std::runtime_error("请提供阶段编号 0-5");
            printStage(tasks, std::stoi(argv[2]), completed);
        } else if (command == "progress") {
            printProgress(tasks, days, completed);
        } else if (command == "done" || command == "undo") {
            std::string date;
            int number{};
            if (argc == 3) {
                date = todayString();
                number = parseTaskNumber(argv[2]);
            } else if (argc == 4) {
                date = argv[2];
                number = parseTaskNumber(argv[3]);
            } else {
                throw std::runtime_error("用法: infra " + command + " [YYYY-MM-DD] <任务编号>");
            }
            const auto& day = requireDay(days, date);
            const bool exists = std::any_of(day.tasks.begin(), day.tasks.end(),
                                            [number](const Task& t) { return t.number == number; });
            if (!exists) throw std::runtime_error("该日期没有这个任务编号");
            const std::string key = taskKey(date, number);
            if (command == "done") completed.insert(key);
            else completed.erase(key);
            saveCompleted(state_path, completed);
            std::cout << (command == "done" ? "Task marked as done!\n" : "Task restored!\n");
            printDay(day, tasks, completed);
        } else {
            printHelp();
            return 1;
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "错误：" << error.what() << "\n";
        return 1;
    }
}
