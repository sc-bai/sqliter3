#pragma once
#include "CppSQLite3U.h"
#include <map>
#include <string>


namespace lsql {
	class CLocalDB {
	public:
		CLocalDB();
		~CLocalDB();
		//����db��ָ�����Ƶı�����
		void ReadAllLine(std::map<int, int>& mpDbInfo, const std::wstring& TblName);

		bool ReadLineByPage(std::map<int, int>& mpDbInfo, const std::wstring& TblName, int index, int pagesize);
		bool InsertLine(const std::wstring& TblName, const int& id, const int& num);
		//����ָ������
		bool UpdateLine(const std::wstring& TblName, const int& id, const int& num);

		//ɾ��ָ������
		bool DeleteLine(const std::wstring& TblName, const int& id);

	private:
		bool Init();
		CppSQLite3DB* m_pSqlDb = nullptr;

	};
}