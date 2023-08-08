
#include <iostream>
#include "localdb.h"

int main()
{
    lsql::CLocalDB* db = new lsql::CLocalDB();
    int id = 0, num = 0;
    //for (int i=0;i<1000;i++)
    //{
    //    db->InsertLine(L"test_table", i + 1, 2 * i + 1);
    //}

    std::map<int, int> data;
   // db->ReadAllLine(data, L"test_table");

    for (int i=0;i<10;i++)
    {
        db->ReadLineByPage(data, L"test_table", i, 20);
		for (auto& item : data) {
			printf("data:id=%d,num=%d\n", item.first, item.second);
		}
        printf("\n");
    }
    

	//for (int i = 0; i < 10; i++)
	//{
 //       if (i % 2 == 0) {
 //           db->DeleteLine(L"test_table", i);
 //       }
	//}
    
    std::cout << "Hello World!\n";
}
