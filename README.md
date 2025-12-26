# muduo_test
使用C++11移植的muduo网络库，使其不再依赖boost库，完全使用现代C++标准库实现。
## 主要特性
零Boost依赖-完全使用C++11标准库替代Boost。
高性能事件循环-Reactor模式，支持多线程TCP服务器。
现代C++语法-使用智能指针、lambda、std::function等现代特性。
简介的API,保持muduo简洁易用的设计哲学。
## 移除的 Boost 组件及替代方案
Boost 组件	C++11 替代方案	说明
boost::function	std::function	函数对象包装器
boost::bind	std::bind / lambda	参数绑定
boost::shared_ptr	std::shared_ptr	智能指针
boost::weak_ptr	std::weak_ptr	弱引用指针
boost::scoped_ptr	std::unique_ptr	独占指针
boost::noncopyable	删除拷贝构造/赋值	不可拷贝基类
boost::ptr_vector	std::vector<std::unique_ptr>	指针容器
## 环境要求
编译器: g++ 4.8+ 或 clang++ 3.3+ (支持 C++11)
操作系统: Linux (推荐 Ubuntu 16.04+) 或 macOS
构建工具: CMake 3.5+
## 构建项目
mkdir build && cd build
cmake..
make
## 致谢
感谢陈硕 (@chenshuo) 创作的优秀 muduo 网络库
参考了 asio、libevent 等优秀网络库的设计思想

