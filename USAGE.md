# AI Infra Roadmap CLI 详细使用说明

`infra` 是一个用 C++17 编写的个人学习路线命令行工具。它把 AI Infra
学习计划拆分到每天，支持查看当天/指定日期/整周任务、勾选完成、撤销完成、
统计阶段进度，以及自动滚动未完成任务。

## 1. 计划范围

- 开始日期：2026-08-05
- 冲刺开始：2026-10-07
- 结束日期：2026-11-25
- 冲刺长度：50 天
- 每日任务：1 项
- 每日预计投入：75 分钟
- 路线总记录：94 天、138 项任务（包含此前保留的88项完成记录）

2026-08-05 至 2026-09-17 的已完成内容原样保留。从 2026-10-07 开始，
每个主题只安排两天：第一天学习知识点，第二天完成一个基础 Lab。旧路线的
Benchmark、报告、README、周复盘和阶段发布任务已经移除。

路线依次覆盖：

1. 系统工程底座
2. 体系结构与性能
3. GPU 与 CUDA
4. 深度学习框架内部
5. LLM 推理系统
6. 分布式与生产化

## 2. 构建程序

### 环境要求

- Windows PowerShell
- CMake 3.15 或更高版本
- 支持 C++17 的 MinGW g++

在项目目录运行：

```powershell
cd C:\ENGLISHpathWORKfile\C++file\infra-roadmap
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\infra.exe init
```

前三条命令生成 `build\infra.exe`，最后一条生成完整路线数据
`data\roadmap.tsv`。

## 3. 启动方式

尚未安装到 PATH 时，可以进入构建目录使用：

```powershell
cd C:\ENGLISHpathWORKfile\C++file\infra-roadmap\build
.\infra.exe today
```

也可以在任意目录使用完整路径：

```powershell
& 'C:\ENGLISHpathWORKfile\C++file\infra-roadmap\build\infra.exe' today
```

不带参数时默认执行 `today`：

```powershell
.\infra.exe
```

## 4. 查看任务

### 查看今天

```powershell
.\infra.exe today
```

如果日期早于 2026-08-05，程序会提示计划尚未开始以及剩余天数。

### 查看指定日期

```powershell
.\infra.exe show 2026-08-05
```

`date` 是 `show` 的别名：

```powershell
.\infra.exe date 2026-08-05
```

日期必须采用 `YYYY-MM-DD` 格式，并位于计划范围内。

### 查看一周

查看某个日期所在的完整计划周：

```powershell
.\infra.exe week 2026-08-08
```

省略日期时查看当前周；如果计划尚未开始，则显示第一周：

```powershell
.\infra.exe week
```

## 5. 完成和撤销任务

每项任务由“原定日期 + 当天任务编号”唯一确定。

完成 2026-08-05 的第 1 项任务：

```powershell
.\infra.exe done 2026-08-05 1
```

当天操作时可以省略日期：

```powershell
.\infra.exe done 1
```

取消完成状态：

```powershell
.\infra.exe undo 2026-08-05 1
```

当天撤销时同样可以省略日期：

```powershell
.\infra.exe undo 1
```

## 6. 延期待办机制

某项任务到了原定日期仍未勾选时，它不会从计划中消失。之后查看任意日期，
程序都会把此前未完成的任务列在“延期待办”区域。

例如在 2026-08-08 查看计划时，未完成的 8 月 5 日任务会显示为：

```text
|2026-08-05#1 [!] 已延迟 3 天（原定 2026-08-05） ...
```

其中：

- `2026-08-05` 是原定日期；
- `#1` 是原定日期当天的任务编号；
- “已延迟 3 天”由查看日期与原定日期实时计算。

完成延期任务时必须使用原定日期，而不是今天的日期：

```powershell
.\infra.exe done 2026-08-05 1
```

完成后，该任务会立即从后续日期的延期待办中消失。延期只是动态显示，程序
不会修改任务原始日期，也不会覆盖路线数据。

## 7. 查看阶段和总体进度

### 所有阶段

```powershell
.\infra.exe stages
```

输出每个阶段的任务完成数、完成比例和预计总时长。

### 指定阶段

```powershell
.\infra.exe stage 2
```

阶段编号如下：

| 编号 | 阶段 |
|---:|---|
| 0 | 系统工程底座 |
| 1 | 体系结构与性能 |
| 2 | GPU 与 CUDA |
| 3 | 深度学习框架内部 |
| 4 | LLM 推理系统 |
| 5 | 分布式与生产化 |

### 总体进度

```powershell
.\infra.exe progress
```

该命令显示：

- 已完成任务数和任务完成率；
- 已经过的计划天数和时间进度；
- 当前周主题；
- 延期任务数量、最早原定日期和最大延期天数；
- 各阶段完成情况。

## 8. 重新生成路线数据

```powershell
.\infra.exe init
```

该命令会根据程序内置课程重新生成 `roadmap.tsv`。它不会主动删除个人完成
状态，但如果修改了源码中的日期或课程结构，旧进度可能无法对应新任务。
正常使用期间不需要反复执行 `init`。

## 9. 文件位置

### 路线数据

开发目录下默认位于：

```text
C:\ENGLISHpathWORKfile\C++file\infra-roadmap\data\roadmap.tsv
```

字段依次为：

```text
date  stage  stage_name  week  topic  task_no  minutes  task
```

### 个人完成状态

Windows 默认位于：

```text
%LOCALAPPDATA%\infra-roadmap\progress.tsv
```

每行保存一个已完成任务键，例如：

```text
2026-08-05|1
```

### 覆盖默认路径

临时指定其他路线文件：

```powershell
$env:INFRA_PLAN_DATA='D:\my-data\roadmap.tsv'
.\infra.exe today
```

临时指定其他进度文件：

```powershell
$env:INFRA_PROGRESS_FILE='D:\my-data\progress.tsv'
.\infra.exe progress
```

这些变量只影响当前 PowerShell 会话，除非你把它们永久写入环境变量。

## 10. 帮助和错误处理

显示内置帮助：

```powershell
.\infra.exe help
```

常见错误：

### 找不到路线数据

```text
找不到路线数据 ... 请先运行 infra init
```

解决方法：回到项目目录执行：

```powershell
.\build\infra.exe init
```

### 日期无效

请确认日期真实存在且格式为 `YYYY-MM-DD`。保留计划为 2026-08-05 至
2026-09-17，冲刺计划为 2026-10-07 至 2026-11-25；中间日期没有任务。

### 任务编号不存在

当前每天只有第 1、2 项任务。延期任务要使用输出中显示的原定日期和编号。

### 中文乱码

建议使用 Windows Terminal 或新版 PowerShell。程序启动时会主动把控制台
输入和输出代码页设置为 UTF-8。

## 11. 命令速查

```text
infra today                  查看今天任务
infra show YYYY-MM-DD        查看指定日期
infra week [YYYY-MM-DD]      查看一周计划
infra stages                 查看所有阶段进度
infra stage <0-5>            查看指定阶段
infra progress               查看总体进度与延期情况
infra done [YYYY-MM-DD] <n>  完成任务
infra undo [YYYY-MM-DD] <n>  撤销完成状态
infra init                   重新生成路线数据
infra help                   显示帮助
```
