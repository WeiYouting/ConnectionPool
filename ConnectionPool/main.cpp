#include <iostream>
#include "Connection.h"
#include "public.h"
#include "ConnectionPool.h"

int main()
{

    Connection  con;
    con.connect("127.0.0.1", 3306, "root", "123456", "test");
    
    //ConnectionPool* pool =  ConnectionPool::getConnectionPool();
    
    clock_t begin = clock();

    std::thread t1([]() {
        for (int i = 0; i < 2500; ++i) {
            //Connection  con;
            //char sql[1024] = { 0 };
            //sprintf(sql, "insert into user(name,age) values ('test',123);");
            //con.connect("127.0.0.1", 3306, "root", "123456", "test");
            //con.update(sql);

            ConnectionPool* pool = ConnectionPool::getConnectionPool();
            std::shared_ptr<Connection> sp = pool->getConnection();
            std::string sql = "insert into user(name,age) values ('test',123);";
            sp->update(sql);
        }
       });

    std::thread t2([]() {


       for (int i = 0; i < 2500; ++i) {
           //Connection  con;
           //char sql[1024] = { 0 };
           //sprintf(sql, "insert into user(name,age) values ('test',123);");
           //con.connect("127.0.0.1", 3306, "root", "123456", "test");
           //con.update(sql);

           
            ConnectionPool* pool = ConnectionPool::getConnectionPool();
            std::shared_ptr<Connection> sp = pool->getConnection();
            std::string sql = "insert into user(name,age) values ('test',123);";
            sp->update(sql);
            
        }
        });

    std::thread t3([]() {

        for (int i = 0; i < 2500; ++i) {
            /*
            Connection  con;
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age) values ('test',123);");
            con.connect("127.0.0.1", 3306, "root", "123456", "test");
            con.update(sql);*/

            ConnectionPool* pool = ConnectionPool::getConnectionPool();
            std::shared_ptr<Connection> sp = pool->getConnection();
            std::string sql = "insert into user(name,age) values ('test',123);";
            sp->update(sql);
        }
        });

    std::thread t4([]() {
        for (int i = 0; i < 2500; ++i) {

            //Connection  con;
            //char sql[1024] = { 0 };
            //sprintf(sql, "insert into user(name,age) values ('test',123);");
            //con.connect("127.0.0.1", 3306, "root", "123456", "test");
            //con.update(sql);

            ConnectionPool* pool = ConnectionPool::getConnectionPool();
            std::shared_ptr<Connection> sp = pool->getConnection();
            std::string sql = "insert into user(name,age) values ('test',123);";
            sp->update(sql);
        }
        });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    
  /*  for (int i = 0; i < 1000; ++i) {
        Connection con;
        char sql[1024] = { 0 };
        sprintf(sql,"insert into user(name,age) values ('test',123);");
        con.connect("127.0.0.1", 3306, "root", "123456", "test");
        con.update(sql);
    }*/

    //for(int i = 0; i < 1000;++i){
    //    ConnectionPool* pool = ConnectionPool::getConnectionPool();
    //    std::shared_ptr<Connection> sp = pool->getConnection();
    //    std::string sql = "insert into user(name,age) values ('test',123);";
    //    sp->update(sql);
    //    
    //}
    clock_t end = clock();
    
    std::cout << end - begin;
    

}
