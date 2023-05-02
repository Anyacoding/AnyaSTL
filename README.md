#  AnyaSTL
通过学习SGI-STL的源码，对STL进行部分复现并尝试拓展新功能
##  目的
- 了解STL底层代码的实现方式
- 利用C++新特性进行实战
- 学习数据结构的实现原理
- 学习现实中算法的实际运用和实现方法

## 编译环境
- C++20
- MinGW w64 9.0

## 第三方库
- GoogleTest
- GoogleMock


## 参考书籍
- 《STL源码剖析》
- 《Effective Modern C++》
- 《C++17完全指南》

# 项目结构
## 空间配置器
负责内存的配置和管理
- [x] allocator
- [ ] shared_ptr
- [ ] unique_ptr
- [ ] weak_ptr

## 迭代器
作为容器和算法的桥梁
- [x] normal_iterator
- [x] reverse_iterator
- [x] iterator_traits
- [x] distance()
- [x] advance()
- [x] next()
- [x] prev()

## 容器
本项目核心
### 序列容器
- [ ] array  
  静态数组
- [x] vector  
  动态数组
- [x] list  
  双向链表
- [ ] deque  
  双端列表
- [ ] heap  
  序列容器上的一系列操作函数
- [ ] forward_list  
  单向链表

### 关联式容器
- [ ] RB-tree  
  红黑树, set和map的基础
- [ ] set  
  有序集合
- [ ] map  
  有序映射
- [ ] multiset  
  有序可重复集合
- [ ] multimap  
  有序可重复映射
- [ ] hashtable  
  哈希表, unordered_set和unordered_map的基础
- [ ] unordered_set  
  无序集合
- [ ] unordered_map  
  无序映射
- [ ] unordered_multiset  
  无序可重复集合
- [ ] unordered_multimap  
  无序可重复映射

## 算法
- [x] 最小/最大操作
- [x] 修改序列的操作
- [ ] 字典序

## 适配器
- [ ] stack  
  栈
- [ ] queue  
  队列
- [ ] priority_queue  
  优先队列

## 代码规范
命名空间：Anya

## 测试
<img alt="" src="https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/anyastl1.png" width="800" style="margin: 0 auto;">
<img alt="" src="https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/anyastl2.png" width="800" style="margin: 0 auto;">
<img alt="" src="https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/anyastl3.png" width="800" style="margin: 0 auto;">
<img alt="" src="https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/anyastl4.png" width="800" style="margin: 0 auto;">
