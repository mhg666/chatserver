#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>
#include<string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

class ChatServer
{
public:
    ChatServer(EventLoop* loop,//循环事件的指针
                const InetAddress& listenAddr,//保存了ip地址和端口号
                const string& _nameArg)//服务器的名字
                :_server(loop,listenAddr,_nameArg)
                ,_loop(loop)
                {
                    //给服务器注册用户连接的创建和断开回调
                    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));//第三个参数是参数占位符，该成员函数有一个参数，所以是_1
                    //给服务器注册用户读写事件的回调 
                    _server.setMessageCallback(std::bind(&ChatServer::oneMessage,this,_1,_2,_3));

                    //设置服务器的线程数量 1个I/O线程 3个worker线程
                    _server.setThreadNum(4);
                }
    void start(){
        _server.start();   
    }
private:
    //专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr &conn)
    {      
        if(conn->connected()){
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->peerAddress().toIpPort()<<"state:online"<<endl;
        }
        else{
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->peerAddress().toIpPort()<<"state:offline"<<endl;
            conn->shutdown();
        }
    }
    //专门处理用户的读写事件
    void oneMessage(const TcpConnectionPtr &conn,//连接
                    Buffer *buffer,//缓冲区
                    Timestamp time)//接收到数据的时间信息
    {
        string buf=buffer->retrieveAllAsString();
        cout<<"recv data:"<<buf<<"time"<<time.toString()<<endl;
        conn->send(buf);
    }
    TcpServer _server;
    EventLoop *_loop;
};

int main(){
    EventLoop loop;
    InetAddress addr("192.168.177.153",9999);
    ChatServer server(&loop,addr,"ChatServer");
    server.start();//将监听描述符添加到epoll上
    loop.loop();//epoll_wait以阻塞方式等待新用户的连接，已连接用户的读写事件等
    return 0;
}




