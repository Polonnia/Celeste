# Celeste Clone

这是一个使用 C++ 和 EasyX 图形库开发的 Celeste 游戏克隆版本。

## 项目结构

```
src/
├── animation/        # 动画相关代码
├── utils/           # 工具函数和辅助类
├── Game.cpp         # 游戏主循环
├── Game.h           # 游戏类定义
├── Level.cpp        # 关卡管理
├── Level.h          # 关卡类定义
├── Player.cpp       # 玩家控制
├── Player.h         # 玩家类定义
├── Render.cpp       # 渲染系统
├── Render.h         # 渲染系统定义
└── main.cpp         # 程序入口
```

## 依赖项

- Visual Studio 2019 或更高版本
- EasyX 图形库
- C++17 或更高版本

## 构建说明

1. 确保已安装 Visual Studio 和 EasyX 图形库
2. 克隆此仓库
3. 使用 Visual Studio 打开解决方案文件
4. 构建并运行项目

## 游戏特性

- 像素风格的平台跳跃游戏
- 流畅的角色动画系统
- 物理引擎支持
- 关卡系统
- 相机跟随

## 控制方式

- 方向键：移动
- Z键：跳跃
- X键：冲刺
- ESC：退出游戏

## 许可证

MIT License 