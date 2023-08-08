#include "localdb.h"

lsql::CLocalDB::CLocalDB()
{
	Init();
}

lsql::CLocalDB::~CLocalDB()
{
	if (m_pSqlDb)
	{
		m_pSqlDb->close();
		delete m_pSqlDb;
		m_pSqlDb = NULL;
	}
}

void lsql::CLocalDB::ReadAllLine(std::map<int, int>& mpDbInfo, const std::wstring& TblName)
{
	try
	{
		wchar_t szCmd[256] = {0};
		wsprintf(szCmd, L"SELECT id,num FROM %s;", TblName.c_str());
		CppSQLite3Query query = m_pSqlDb->execQuery(szCmd);//执行查询语句
		while (!query.eof())
		{
			int id = query.getIntField(L"id");    //列项为id的值
			int testnum = query.getIntField(L"num");    //列项testnum的值

			mpDbInfo.insert(std::make_pair(id, testnum));//插入map
			query.nextRow();//继续下一行
		}
		query.finalize();//结束查询,释放内存
	}
	catch (CppSQLite3Exception& e)
	{
		return;
	}
}

bool lsql::CLocalDB::ReadLineByPage(std::map<int, int>& mpDbInfo, const std::wstring& TblName, int index, int pagesize)
{
	//SELECT* FROM 表名 LIMIT pageSize OFFSET(pageIndex * pageSize);
	try
	{
		mpDbInfo.clear();
		wchar_t szCmd[256] = { 0 };
		wsprintf(szCmd, L"SELECT id,num FROM %s order by id limit %d offset %d;", TblName.c_str(), pagesize, index*pagesize);
		CppSQLite3Query query = m_pSqlDb->execQuery(szCmd);//执行查询语句
		while (!query.eof())
		{
			int id = query.getIntField(L"id");			//列项为id的值
			int testnum = query.getIntField(L"num");    //列项testnum的值

			mpDbInfo.insert(std::make_pair(id, testnum));//插入map
			query.nextRow();//继续下一行
		}
		query.finalize();//结束查询,释放内存
		return true;
	}
	catch (CppSQLite3Exception& e)
	{
		return false;
	}

	return false;
}

bool lsql::CLocalDB::InsertLine(const std::wstring& TblName, const int& id, const int& num)
{
	try
	{
		wchar_t szCmd[256];
		wsprintf(szCmd, L"insert into %s (id, num) values (%d, %d)", TblName.c_str(), id, num);//更新内容
		m_pSqlDb->execDML(szCmd);
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}
	return TRUE;
}

bool lsql::CLocalDB::UpdateLine(const std::wstring& TblName, const int& id, const int& num)
{
	try
	{
		wchar_t szCmd[256];
		wsprintf(szCmd, L"update %s set num = %d WHERE id=%d;", TblName.c_str(), num, id);//更新内容
		m_pSqlDb->execDML(szCmd);
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}
	return TRUE;

}

bool lsql::CLocalDB::Init()
{
	if (m_pSqlDb || !(m_pSqlDb = new CppSQLite3DB))
	{
		return FALSE;
	}

	try
	{
		std::wstring strDbFile = L"D:\\data.s3db";
		m_pSqlDb->open(strDbFile.c_str());

		std::wstring strSql = L"CREATE TABLE IF NOT EXISTS test_table(id integer PRIMARY KEY AUTOINCREMENT, num integer NOT NULL)";
		try
		{
			m_pSqlDb->execDML(strSql.c_str());
		}
		catch (...) {

		}
	}
	catch (CppSQLite3Exception& e)//处理sqlite异常
	{
		return FALSE;
	}

	return TRUE;

}

bool lsql::CLocalDB::DeleteLine(const std::wstring& TblName, const int& id)
{
	try
	{
		wchar_t szCmd[256];
		wsprintf(szCmd, L"DELETE FROM %s WHERE id=%d;", TblName.c_str(), id);//删除语句
		m_pSqlDb->execDML(szCmd);
	}
	catch (CppSQLite3Exception& e)
	{
		return FALSE;
	}
	return TRUE;

}
