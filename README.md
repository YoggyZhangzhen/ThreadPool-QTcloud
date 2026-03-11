# ThreadPool-QTcloud
## 配置

我只记重要的：

在服务器和客户端（配置pro文件），创建config文件保存配置信息（在我这里保存的是ip和port），然后用QFile对象读取内容（QByteArray），把它转Qstring类然后按照制定规则（比如\r\n）split成一个string的列表

目的：==得到端口号和ip==

## 实现服务器和客户端类

实现服务器类和客户端类，两个类型都实现单例模式。（删除拷贝和赋值，用一个静态函数返回实例

==面试点1：单例模式==->扩展：设计模式（工厂模式

然后服务器发送一个socket给客户端，这个socket用来携带信息PDU,客户端接收

然后双方之间就可以用这个socket来交互

这个pdu由总长，数据长，参数（这个参数就是用户的账号密码，64字节，在数据库中以两个32字节varchar变量保存，执行注册逻辑时，在客户端的lineedit输入，设置类型为注册，然后用memcpy传入自己构建的pdu的cadata，然后pdu发送给server，然后server与数据库交互调用注册函数(不存在该用户就插入)，返回成功或者失败将bool变量存入cadata返回给客户端，QMessage弹窗提示），类型，数据组成，是一个柔性数组(最后一个元素是一个未定义长度的数组，sizeof得到的结果不包括那个数组)，所以

==面试点2：柔性数组->扩展：数据链路层帧定界，tcp粘包，tcp首部长度字段==

![image-20250119102216527](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250119102216527.png)

收到pdu后读取就要依赖总长，先读取总长，才能知道柔性数组长度，然后指针偏移sizeof(uint)位，因为我这里的总长是uint类，从当前位置开始读，就可以得到其他四个成员变量了

但是后续如果一次性发送==多个请求==仍然存在着粘包问题，因此要用==循环读取==解决

服务器可以接收来自多个客户端的socket，在listen时每收到一个socket，调用incomingconnection（==已经被线程池优化，见下面==），在函数内根据文件描述符区分每一个socket，我们也实现了一个自己的socket用于扩展

## 数据库操作

在数据库方面，我实现了一个数据库操作类（基于mysql

一点小问题：连接数据库，直接把mysql里面的驱动文件放到对应的bin和plugins里面的sqldrivers目录下

==面试点3：槽函数==

逻辑：通过==槽函数触发请求==，client发送对应的pdu给server，根据类型进行响应，比如是否需要调用数据库，如果需要与数据库交互，返回内容给客户端，客户端显示结果或者处理得到的消息

## 注册登录下线

登录的sql语句和注册不一样，变成了查询有没有对应的项目，以及更新在线状态，==注册成功创建用户同名文件夹==

下线：1，断开连接时，disconnected信号连接（mytcpsocket里的online置0）槽函数，根据在登录时读取的name参数确定作为函数参数，找到这个数据项，使得数据库方面数据库方面：online置0；2，服务器方面：下线的socket应该调制mmytcpserver在socket列表删除，在socket自定义一个下线信号（无需实现）和（mytcpserver里的用于list减少元素的）槽函数

## 主界面搭建

设计三个新设计师界面类，将friend和file加入index

因为ui类是私有的，为了在index中得到公有的friend和file对象就要创建公有函数

登录成功后通过调用Index的单例，显示主界面隐藏登录界面

## 查找用户,在线用户

查找用户：点击选项后在文本框中写客户端发要查的妹子，作为pdu参数传给服务器查找用户在线状态（在数据库查找online），返回不同值作为pdu参数给client，显示用户状态

在线用户：请求后得到在数据库取出满足的数据项存服务器到qstringlist（每个32长度）里面，返回这个stringlist到客户端算出长度（每个32），从而for循环取出每一个msg(每次+32字节)（存到总算用到camsg了，用来存储用户名列表显示在线用户）

==问：为什么要在friend类里面放一个在线表指针== 答：因为要基于friend页面创建一个子界面

## 添加好友

1，主动添加方客户端在在线名单点击对方名字添加槽函数提==取当前客户端用户的name和被点击用户name打包到pdu==发给server

2，遍历socket列表，根据名字找到对应的客户端，==在其socket中写入发来的pdu==以便转发给客户端2，==在数据库找到查询两者是否是好友和在线状态==，若都在线且不是好友发送好友请求，将==添加请求类pdu==发送给客户端2，将是否能添加的==添加响应类pdu==反馈给客户端1，前者参数是==起点终点==，后者是==返回值==(用于显示能不能加嘛)

3，客户端2收到agree请求类型pdu决定是否同意添加，不同意直接返回，同意返回一个==起点终点参数pdu（agree_request）==给服务器;服务器收到后==在数据库添加一项数据==，返回一个==agree_respond给客户端1（bool参数pdu）==;客户端根据收到的服务器bool参数==显示加没加上==

处理加好友那个服务器转发的应该是请求，而不是正常在客户端的响应

查找用户添加好友，只要在到客户端2的findresponsepdu带上返回值和客户端1的名字即可

## 刷新好友,聊天

客户端发出此类请求，服务器在数据库查找friendid或者userid满足的且在线的，返回给客户端，每次按钮刷新

设计一个chatui，在chat类里设计一个名字属性代表和谁聊天，friend类里增加一个chat指针成员变量，判断是否选择了好友，显示聊天界面。客户端1发消息过去后，服务器转发給2，2接收请求而不是响应然后转发回去

## 创建文件夹，刷新文件，粘包，删除文件夹

用户注册的时候在项目目录创建文件夹，然后再config里面加入配置。

创建文件夹：通过输入框获取文件名把文件名存入参数cadata，当前路径存入数据camsg，发送给服务端，服务端拼接后判断文件是否存在，不存在就创建，返回响应ret参数pdu

刷新文件：传当前路径参数pdu给服务器，服务器获取文件过滤掉.和..发送回去camsg里面放内容返回，客户端把pdu的数据拿出来显示，这里有一个问题，就是==刷新好友和刷新文件两个pdu需要同时读取，粘包了==，然后客户端服务器都要加，不加还是有bug

pdu传过去（因为弹窗只有名字，不能看出类型），客户端遍历文件去判断是文件还是文件夹，可以就发送了，camsg里面放当前目录+文件名

## 进入，返回，移动文件夹

进入就把双击的文件夹名字作为curpath后面的filename，刷新

返回找到最后一个/，删后面的，然后获取新的路径

当前的文件名和移动前路径作为成员变量通过点击的文件读取名字，移动前路径为cur路径+被点击文件名，目标为cur路径+被点击文件夹+前面那个文件名，客户端发送的pdu，==参数是（起地址长度，终地址长度），camsg：起始终止地址+'\0'== 返回是否成功的pdu

## 上传文件

这一次还要发送一个文件大小属性，在file类里面存一个选择上传的文件名属性保存，

==细节：==可以用lastindexof函数配合remove或者right函数（传入要右边几个元素）找到最后一个/然后提取文件名，算路径长度等

客户端主动发送的pdu：参数：文件名，文件大小；msg：文件当前路径

服务端进行初始化，根据发来的pdu确认要上传的文件位置，然后对==上传size变量==和==已经接收size变量==初始化分别为filesize和0，根据路径初始化创建文件；

客户端接收到如果==成功就可以传输文件内容（上面一步只是创建文件==

面试点：==这个和其他的pdu传送不一样，因为是 a->b->a->b->a而不是a->b->a==

客户端二次传送：msg里面放文件数据 ，循环读取，要==设置pdu的长度==，因为最后一次读取长度不一定是固定的（4096）

服务器二次返回：接收到长度是否等于上传size的bool参数pdu

==面试点：connect的第五个参数==

==面试点：断点续传==

## 多线程

### 客户端发送数据时的多线程

建立了一个处理上传类，对于原来的弹窗等，因为不能确定是父子线程哪个调用，用emit一个信号替换

开始和上传槽函数关联，上传过程中的两个信号被client接收负责转发pdu和显示弹窗，上传完毕发送finished信号和quit函数关联，然后完成后移除线程

### 服务器端的线程池

继承runnable建立一个客户端接收类，重写run方法关联线程，线程池放在tcpserver类，每来一个socket接收一个

### 分享

创建一个新页面关联file页面，pdu参数存主动方用户名和好友数量，msg存所有好友名字+路径

服务器==转发==给所有好友，msg只需要放路径（偏移numfriend*32），转发给客户端2同时响应给客户端1（这个pdu无参无msg）

客户端2收到转发就在pdu加入文件名到msg和==被动方名字==放入返回给服务器

然后客户端 路径+用户名+msg（文件名）copy文件，返回成功与否给客户端1

pip install opencv-python -f https://mirror.sjtu.edu.cn/pytorch-wheels/torch_stable.html
![image-20250306184107584](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250306184107584.png)

# 线程池项目

==并发编程是大厂热门！！==

## 多线程使用时机

IO密集型，设备文件网络操作，（等待客户端连接，IO操作是可以把程序阻塞住的），在这样给分配给CPU时间片，CPU相当于空闲下来了

操作系统在就绪队列找任务，阻塞队列放的任务在等待资源；所以IO密集型更加适合设计成多线程程序

多核适合io和cpu密集型；单核适合io密集，单核如果多线程会有上下文切换的开销

## 线程真的越多越好？

线程的创建和销毁都是非常“重”的操作

线程栈本身占用大量内存；创建了一大批线程，还没有做具体的事情，每一个线程都需要线程栈，栈几乎都占用完了，还怎么做事

线程的上下文切换要占用大量时间；上下文切换的花费的CPU时间也特别多

## 为什么要线程池

不应该实时创建线程

## fixed和cached

固定个数

动态增长，根据线程数量动态增长，但是有一个阈值，如果空闲60s就关闭线程

## 线程同步方式

### 线程互斥

互斥锁，原子类型

能不能在多线程执行->看这段代码是否存在==竞态条件== ->这段代码就是临界区代码段-->可以保证它的原子操作

多线程环境下不存在竞态条件->可重入

存在竞态条件->不可重入



CAS操作（无锁机制） 无锁队列，无锁链表，无锁数组



线程共享heap和数据段

cnt++不是原子操作，看似增加两次，cnt只+1

![image-20250307185243607](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250307185243607.png)

### 线程通信

#### 条件变量

condition_variable一般和mutex一起使用

![image-20250307194931271](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250307194931271.png)

线程互斥是在一个代码块里能不能进来的问题

通信是不同代码块的依赖关系

==C++默认是没有考虑多线程，容器都不是线程安全的==

要用unique_lock!

为什么不用lock_guard而是unique_lock？

前者在构造函数获取锁，在析构函数释放，太简单了

wait：改变线程状态；释放锁

![image-20250307203014205](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250307203014205.png)

等待->阻塞 notify了等待变阻塞，拿到锁才从阻塞变成就绪

notify_one而不是all的话可能导致死锁

#### 信号量

 semaphore

看作资源计数没有限制的mutex互斥锁

![image-20250307205217596](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250307205217596.png)

信号量和互斥锁不一样的就是，互斥锁只能由上锁的解锁

## 智能指针

因为task可能被用户提前析构，所以要用智能指针sharedptr；正常指针如果被析构就变成野指针，可能其他也指向这里的指针把它释放了；shared_ptr不会有这种情况，因为本身一直有一个指向那块地址，引用计数存在

## 业务逻辑：开始线程

记录初始线程个数，然后创建线程对象，启动所有线程（线程的start）

## emplace_back

emplace_back 是 std::vector 等顺序容器提供的一个方法，用于在容器的尾部直接构造元素。与 push_back 不同，emplace_back 可以直接接受构造元素所需的参数，在容器内部直接构造元素，而不是先创建一个临时对象，再将其移动或拷贝到容器中。

已经有一个对象要放进去用push_back

## 绑定器

```c++
// 一个简单的加法函数
int add(int a, int b) {
    return a + b;
}

int main() {
    // 使用 std::bind 固定第一个参数为 10
    auto addTen = std::bind(add, 10, std::placeholders::_1);
    // 调用新的可调用对象，只需传入一个参数
    std::cout << addTen(5) << std::endl;  // 输出 15
    return 0;
}
```

## 类型别名

```c++
using ThreadFunc = std::function<void(int)>;
```

`using` 关键字可用于为已有的类型定义一个新的名字

==ThreadFunc==` 是新定义的类型别名，它代表的实际类型是 `std::function<void(int)>`。

==std::function==` 是 C++ 标准库 `<functional>` 头文件中提供的一个通用的多态函数包装器，它是一个类模板。其主要作用是存储、复制和调用任何可调用对象

## 分离线程

```c++
void Thread::start() {
	// 创建一个线程来执行一个线程函数
	std::thread t(Func_,threadId_); //c++11来说 分为线程对象t 和线程函数Func_
	t.detach(); //设置分离线程
	//为什么要设置分离线程？
	// 因为如果不分离的话 线程对象t和执行线程函数Func_的线程是具有绑定关系的
	// 当这个start函数执行完后 线程对象t离开作用域 线程对象t会被析构 也就是执行线程函数的这个线程也终止了
```

t死掉，func不死

## 提交任务

获取锁，如果队列没有空余在notFull进行wait

## 开始



![image-20250308155919898](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308155919898.png)在作用域外这个锁就析构了

满足条件且拿到锁之后执行任务

提交失败的场景：如果任务队列为4，提交了8个（很多任务），会有提交失败的情况（有1s等待时间，超过就放弃任务

![image-20250308161744104](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308161744104.png)

为什么要返回值

模板函数和虚函数不能写到一块，但是我们这里要一个返回值，这就不能在模板中实现多态的返回值

1，怎么设计run函数的返回值，表示任意类型

2，如何设计result机制，没有执行完阻塞，执行完返回结果

需要上帝类。类似object

## ANY类型

基类指针可以指向任何派生类，所以Any需要有一个基类指针

巧妙的any==，对any传入一个模板类参数初始化，因为类型不同，先把这个模板传给派生类对象，然后用这个派生类对象对一个大基类对象初始化==

要禁止拷贝构造，因为只能有一个指针指向这个资源

右值引用拷贝构造开放

## 信号量

```c++
//获取一个信号量资源
void  Semaphore::wait() {
	std::unique_lock<std::mutex> lock(mtx_);
	// 等待信号量有资源，没有资源的话，会阻塞当前线程
	cond_.wait(lock, [&]()->bool {return resLimit_ > 0; });
	resLimit_--;
}

// 增加一个信号量资源
void Semaphore::post() {
	std::unique_lock<std::mutex> lock(mtx_);
	resLimit_++;
	cond_.notify_all();
}
```



## 返回值考虑

result依赖于task，task被执行完，task对象没了，所以这时的依赖于task对象的result对象也没了

![image-20250308215651132](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308215651132.png)

问题

![image-20250308220330133](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308220330133.png)

把setVal和run执行关联起来，增加了一个exec方法，在exec中对run发生多态调用，因为直接this->run()虚函数没有发生多态调用

强智能指针会存在==交叉引用==问题，所以task里面不能再有result指针

res指针的赋值在创建result对象的时候在task智能指针里面实现

![image-20250308221438117](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308221438117.png)

## Master-Slave模型

![image-20250308230119535](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250308230119535.png)

整体流程：submitTask里面放自定义的SharedTask指针，在线程池的threadFunc里面消费任务，调用run，返回res，通过获取res的any对象转换成相应的类型。为什么要有返回值和any两个类？any实现万能事物，返回值可能不能返回

## cache的问题

1，用户设置模式

要防止弱智用户在start后setmode，加一个bool，在start的时候设置为true

2，需要根据任务数量和空闲线程梳理判断是否要新的线程

cach模式，任务量大于空闲线程，现在线程数量小于threshold

3，空闲时间超过60s，回收

获取线程id，在线程池找到需要析构的线程

原来用vector，找不到对应的对象，所以现在用map

![image-20250309002304463](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309002304463.png)

## 主线程回收时候按回车没反应的问题

当执行 `threads_.erase(_threadid);` 时，容器会从内部移除键为 `_threadid` 的元素。由于容器中存储的是 `std::unique_ptr<Thread>`，移除元素会导致该 `std::unique_ptr` 被销毁。根据 `std::unique_ptr` 的特性，当它被销毁时，其所指向的 `Thread` 对象的析构函数会被调用，从而释放 `Thread` 对象所占用的内存空间。

走的时候要通知一下，不然析构函数的wait就一直卡住了

## 问题

实现资源回收的时候出现了死锁的问题。

1.notEmpty的线程被通知了；

2.如果是run的线程，ispoolrunning已经设置成false，在while外面也能正常删除

3.哪一种线程不能正常删除呢？要析构的时候，进程已经进入while(ispoolrunning)循环了

析构函数释放taskQueMtx锁，ispoolRunning循环里面获取这个锁，但是此时没任务，notEmpty条件变量就wait了，没有人对于他notify

线程池里的线程先获取锁，然后往下到notEmpty那里也堵住了

最终问题点在线程池里的线程在notEmpty条件wait了 但是没人唤醒

![image-20250309115712597](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309115712597.png)

![image-20250309115247545](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309115247545.png)

线程池里的线程通知notEmpty，然后先获取锁，右边就能拿到锁，然后通过wait函数

如果右边先获取锁，释放之后，左边也可以获取锁，然后notify，左边通知完后wait不满足释放锁，右边获取锁，继续往下，然后因为已经析构ispoolrunning变成了false，break

![image-20250309115906158](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309115906158.png)![image-20250309120054961](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309120054961.png)

![image-20250309115947255](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309115947255.png)

后获取，线程池已经结束

总体梳理问题：

线程池析构的时候，同时有一个线程在析构前把ispoolrunning这个变量设置为false（循环处理任务的判断逻辑）之前就已经进入了循环，从而导致两种情况，一种是这个线程提前拿到任务锁，而==我们的代码逻辑是先通知所有线程任务为空，再对任务锁进行获取，然后等待线程释放完，==那么就会导致什么问题呢？循环中的那个线程如果1，先获取任务锁，会卡在下面的任务队列非空就不能往下执行逻辑，没有成功继续往下释放线程；2，如果后获取，得到锁之后依然卡在那里；所以我们需要循环里的那个==锁在释放后还能得到一个信号==，后者后抢到锁的时候能接收到一个信号。 同时我们还需要锁+双重循环有助于break出这个根据任务队列大小确定的for循环

notify_all是把正在wait的条件变量唤醒！！！！

![image-20250309121532170](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309121532170.png)

## 问题1.5![image-20250309121903536](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309121903536.png)

原因：任务还没执行完，线程池就被释放了

所有任务必须执行完成，才可以回收线程资源；双重判断得去掉；变成仍然是任务队列的size==0；如果ispoolrunning是false，退出；

如果用ispoolrunning大循环会提前退出，要在任务size==0的时候，再进行判断

![image-20250309122445359](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309122445359.png)

## 问题2

放到linux上又死锁了，为啥呢

用gdb拉起来一个attach的正在运行的死锁进程，然后info thread之后发现确实所有线程都在wait，然后通过bt查看各个线程调用堆栈，通过thread id号去查看各个线程的情况，分析了一下，发现主线程正常，子线程有问题信号量notify之后lock住了

原因：任务执行返回的result对象被析构了，在vs下，==条件变量析构会释放相应资源；但是linux上的g++库没有实现这个功能，linux的条件变量析构函数啥也没做！！！！==

信号量源码加一个bool值，初始化为false，如果被析构了就不要notify了

![image-20250309131802762](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309131802762.png)

**![image-20250309131859204](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309131859204.png)**

## packaged和future机制

![image-20250309135852915](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309135852915.png)

future来代替result（+信号量+task，任务不执行完毕，不能返回

![image-20250309140714737](C:\Users\20167\AppData\Roaming\Typora\typora-user-images\image-20250309140714737.png)

通过task的构造函数setResult，在这个函数给task绑定一个result，在submitTask的时候通过调用start然后进入线程的start然后进入threadFunc通过task的exec里面result_->setVal(run());调用run里面的给result赋值。

信号量在哪里起作用：信号量通过mutex和conditionvariable实现，，初始资源计数为0，post增加信号量，wait释放，setVal设置的时候要阻塞get方法，就是直到result赋值前都不能使用wait释放资源。

这里是一个二元信号量，确实和mutex类似，==信号量的post会唤醒所有等待线程，互斥锁的unlock只会唤醒一个等待线程就是他自己！！==

## ANY类（多态应用

类型代码

类型擦除==运行时==拥有==不同的具体类型==，但==编译==的时候可以通过一个==统一的接口==操作

```c++
class Any
{
public:
	Any() = default;
	~Any() = default;
	Any(const Any&) = delete;
	Any& operator=(const Any&) = delete;
	Any(Any&&) = default;
	Any& operator=(Any&&) = default;

	// 这个构造函数可以让Any类型接收任意其它的数据
	template<typename T>  // T:int    Derive<int>
	Any(T data) : base_(std::make_unique<Derive<T>>(data))
	{}

	// 这个方法能把Any对象里面存储的data数据提取出来
	template<typename T>
	T cast_()
	{
		// 我们怎么从base_找到它所指向的Derive对象，从它里面取出data成员变量
		// 基类指针 =》 派生类指针   RTTI
		Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
		if (pd == nullptr)
		{
			throw "type is unmatch!";
		}
		return pd->data_;
	}
private:
	// 基类类型
	class Base
	{
	public:
		virtual ~Base() = default;
	};

	// 派生类类型
	template<typename T>
	class Derive : public Base
	{
	public:
		Derive(T data) : data_(data)
		{}
		T data_;  // 保存了任意的其它类型
	};

private:
	// 定义一个基类的指针
	std::unique_ptr<Base> base_;
};
```

### 构造函数

构造析构正常，左值赋值=和拷贝构造删除，右值=和移动构造留下，这些是为了==保持unique_ptr的独占性==

```c++
Any() = default;
~Any() = default;
Any(const Any&) = delete;
Any& operator=(const Any&) = delete;
Any(Any&&) = default;
Any& operator=(Any&&) = default;
```

```c++
// 这个构造函数可以让Any类型接收任意其它的数据
template<typename T>  // T:int    Derive<int>
Any(T data) : base_(std::make_unique<Derive<T>>(data))
{}
```

这个我们先看base_成员的情况，传递类型进来的时候编译器会自动推断类型，用derived类的一个对象初始化base指针

==不同子类对象都可以赋值给父类指针==，所以以此想到可以用多态实现any类型赋值

#### 为什么不能直接用一个base类赋值？

如果直接在base里面实现模板类，然后赋值给_base,这个DERIVED<T>不一定是啥呢

### 数据提取

```c++
// 这个方法能把Any对象里面存储的data数据提取出来
template<typename T>
T cast_()
{
    // 我们怎么从base_找到它所指向的Derive对象，从它里面取出data成员变量
    // 基类指针 => 派生类指针   RTTI
    Derive<T>* pd = dynamic_cast<Derive<T>*>(base_.get());
    if (pd == nullptr)
    {
        throw "type is unmatch!";
    }
    return pd->data_;
}
```

### 内部类

定义一个基类和派生类

```c++
// 基类类型
class Base
{
public:
    virtual ~Base() = default;
};

// 派生类类型
template<typename T>
class Derive : public Base
{
public:
    Derive(T data) : data_(data)
    {}
    T data_;  // 保存了任意的其它类型
};
```

这个虚析构函数是为了防止内存泄漏，因为构造的时候有参构造成员是一个父类指针

Derived类里面有一个有参构造，存储数据data，这个data类型是模板T类型

### 成员

```c++
std::unique_ptr<Base> base_;
```

定义了一个基类指针

### 如何实现万能类

any类的构造函数中base指针用一个derived附带模板类型的指针进行初始化

## 信号量类

```c++
class Semaphore
{
public:
	Semaphore(int limit = 0)
		:resLimit_(limit)
	{}
	~Semaphore() = default;

	// 获取一个信号量资源
	void wait()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		// 等待信号量有资源，没有资源的话，会阻塞当前线程
		cond_.wait(lock, [&]()->bool {return resLimit_ > 0; });
		resLimit_--;
	}

	// 增加一个信号量资源
	void post()
	{
		std::unique_lock<std::mutex> lock(mtx_);
		resLimit_++;
		// linux下condition_variable的析构函数什么也没做
		// 导致这里状态已经失效，无故阻塞
		cond_.notify_all();  // 等待状态，释放mutex锁 通知条件变量wait的地方，可以起来干活了
	}
private:
	int resLimit_;
	std::mutex mtx_;
	std::condition_variable cond_;
};
```

提供get，post两个接口，get

## Task类


<img width="378" height="184" alt="image" src="https://github.com/user-attachments/assets/58ecb45c-8802-40da-afd0-ba09499b9c95" />

## Result类

## 线程类

## 线程池类





