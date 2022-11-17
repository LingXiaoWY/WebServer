#include "../include/sql_connection_pool.h"

connection_pool::connection_pool() : m_CurConn(0), m_FreeConn(0) {}

connection_pool::~connection_pool()
{
    DestroyPool();
}

MYSQL *connection_pool::GetConnection() //获取数据库连接
{
    MYSQL *con = nullptr;

    if (connList.size() == 0)
    {
        return nullptr;
    }

    reserve.wait();

    lock.lock();

    con = connList.front();
    connList.pop_front();

    --m_FreeConn;
    ++m_CurConn;

    lock.unlock();

    return con;
}

bool connection_pool::ReleaseConnection(MYSQL *conn) //释放连接
{
    if (conn == nullptr)
        return false;

    lock.lock();

    connList.push_back(conn);
    ++m_FreeConn;
    --m_CurConn;

    lock.unlock();

    reserve.post();
    return true;
}

int connection_pool::GetFreeConn() //获取连接
{
    return this->m_FreeConn;
}

void connection_pool::DestroyPool() //销毁所有连接ls
{
    lock.lock();
    if (connList.size() > 0)
    {
        list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); ++it)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

//单例模式
connection_pool *connection_pool::GetInstance()
{
    static connection_pool connPool;
    return &connPool;
}

//构造初始化
void connection_pool::init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log)
{
    m_url = url;
    m_Port = Port;
    m_User = User;
    m_DatabaseName = DataBaseName;
    m_PassWord = PassWord;
    m_close_log = close_log;

    for (int i = 0; i < MaxConn; i++)
    {
        MYSQL *con = nullptr;
        con = mysql_init(con);

        if (con == nullptr)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }
        con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DataBaseName.c_str(), Port, nullptr, 0);
        if (con == nullptr)
        {
            LOG_ERROR("MySQL Error");
            exit(1);
        }
        connList.push_back(con);
        ++m_FreeConn;
    }
    reserve = sem(m_FreeConn);

    m_MaxConn = m_FreeConn;
}

connectionRAII::connectionRAII(MYSQL **con, connection_pool *connPool)
{
    *con = connPool->GetConnection();
    conRAII = *con;
    poolRAII = connPool;
}

connectionRAII::~connectionRAII()
{
    poolRAII->ReleaseConnection(conRAII);
}
