# AI Infra Roadmap CLI

一个延续 `todolist` 使用习惯的 C++17 命令行学习计划工具。路线从
2026-08-05 开始，共 49 周；每天安排 60–90 分钟，目标是在 2027 年暑期
实习投递前完成系统、CUDA、深度学习框架、LLM 推理、分布式与部署训练。

完整的构建、命令、延期机制、数据文件和故障排查说明见
[USAGE.md](USAGE.md)。

当天未勾选的任务会持续显示在后续日期的“延期待办”中，并标明原定日期和
已延迟天数。延期任务仍使用原日期和任务编号完成，例如：

```text
infra done 2026-08-05 1
```

## 构建

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\infra.exe init
```

`init` 会在 `data/roadmap.tsv` 生成完整的每日路线数据。

## 常用命令

```text
infra today
infra show 2026-08-01
infra week 2026-08-01
infra stages
infra stage 2
infra progress
infra done 2026-08-01 1
infra undo 2026-08-01 1
infra help
```

不带参数时等同于 `infra today`。`done` 和 `undo` 省略日期时默认今天：

```text
infra done 1
infra undo 1
```

## 文件

- `data/roadmap.tsv`：程序生成的路线数据，一项任务一行。
- `%LOCALAPPDATA%\infra-roadmap\progress.tsv`：个人完成状态。
- `INFRA_PLAN_DATA`：可覆盖路线数据文件路径，便于测试或迁移。
- `INFRA_PROGRESS_FILE`：可覆盖进度文件路径。

程序查找路线数据的顺序是：环境变量、可执行文件同级、可执行文件上一级的
`data`、当前目录的 `data`。因此开发目录和以后复制到工具目录两种方式都支持。
