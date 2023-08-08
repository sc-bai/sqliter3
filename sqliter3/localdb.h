#pragma once
#include "CppSQLite3U.h"
#include <map>
#include <string>


namespace lsql {
	class CLocalDB {
	public:
		CLocalDB();
		~CLocalDB();
		//读出db中指定名称的表数据
		void ReadAllLine(std::map<int, int>& mpDbInfo, const std::wstring& TblName);

		bool ReadLineByPage(std::map<int, int>& mpDbInfo, const std::wstring& TblName, int index, int pagesize);
		bool InsertLine(const std::wstring& TblName, const int& id, const int& num);
		//更新指定数据
		bool UpdateLine(const std::wstring& TblName, const int& id, const int& num);

		//删除指定数据
		bool DeleteLine(const std::wstring& TblName, const int& id);

	private:
		bool Init();
		CppSQLite3DB* m_pSqlDb = nullptr;

	};
}