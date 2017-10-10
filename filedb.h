#pragma once
//Created by Microsoft Visual Studio.
//User: zqx
//Date : 2017 / 10 / 2
//Time : 15 : 13
//To change this template use File | Settings | File Templates.
using namespace std;
#include<string>
#include<vector>
#include<fstream>
//数据库CRUD操作方法封装
class FileDB
{
public:
	
	//选择数据，DB_NAME为表名，entity为选择条件，VALUES为选择字段，resultSet为结果集
	//操作成功，返回受影响的行数，操作失败，返回-1
	template<typename T>
	static int select(string DB_NAME, T& entity, vector<string>& VALUES, vector<T>& resultSet)
	{
		ifstream readFile;
		try {
			T temp;
			readFile.open(DB_NAME + ".dat", ios::_Nocreate);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			//SELECT * FORM DB_NAME
			if (VALUES[0] == "all") {
				while (readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
					//去除id为-1的无效数据
					if (temp.id != -1)
						resultSet.push_back(temp);
				}
				readFile.close();
			}
			//SELECT * FORM DB_NAME WHERE......
			else {
				while (readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
					//去除id为-1的无效数据
					if (temp.id != -1) {
						int flag = 1;
						//遍历选择条件
						for (int i = 1; i < VALUES.size(); i++) {
							if (temp.getElemType(VALUES[i]) == "int") {
								if (temp.getIntElemByName(VALUES[i]) != entity.getIntElemByName(VALUES[i])) {
									flag = 0;
									break;
								}
							}
							else if (temp.getElemType(VALUES[i]) == "float") {
								if (temp.getFloatElemByName(VALUES[i]) != entity.getFloatElemByName(VALUES[i])) {
									flag = 0;
									break;
								}
							}
							else {
								if (strcmp(temp.getCharElemByName(VALUES[i]), entity.getCharElemByName(VALUES[i]))) {
									flag = 0;
									break;
								}
							}
						}
						//符合条件，加入结果集
						if (flag)
							resultSet.push_back(temp);
					}
				}
				readFile.close();
			}
			return resultSet.size();
		}//捕获文件操作异常，返回-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}
	}

	//增加数据，DB_NAME为表名，entity为待增加实体
	//操作成功，返回受影响的行数，操作失败，返回-1
	template<typename T>
	static int insert(string DB_NAME, T& entity)
	{
		int id;
		ofstream writeFile;
		ifstream readFile;
		try {
			//读取info文件，获取最后一条数据id，实现PK自增功能
			readFile.open(DB_NAME + "Info.dat", ios::_Nocreate);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			readFile.read((char*)&id, sizeof(id));
			readFile.close();
			//PK自增
			id++;
			//25、26的十六进制0x1a、0x1b会触发文件结尾EOF(),所以跳过
			if (id == 25) id += 2;
			entity.setId(id);
			//自增PK写入文件
			writeFile.open(DB_NAME + "Info.dat", ios::_Nocreate);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.write((char*)&id, sizeof(id));
			writeFile.close();
			//数据插入
			writeFile.open(DB_NAME + ".dat", ios::app | ios::_Nocreate);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.write((char*)&entity, sizeof(entity));
			writeFile.close();
			return 1;
		}//捕获文件操作异常，返回-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}
	}

	//删除数据，DB_NAME为表名，entity为选择条件，VALUES为选择字段
	//操作成功，返回受影响的行数，操作失败，返回-1
	template<typename T>
	static int Delete(string DB_NAME, T& entity, vector<string>& VALUES)
	{
		ofstream writeFile;
		ifstream readFile;
		int res = 0;
		//DELETE * FORM DB_NAME
		try {
			if (VALUES[0] == "all") {
				writeFile.open(DB_NAME + ".dat", ios::_Nocreate);
				if (!writeFile) {
					ios_base::failure fail("ERROR");
					throw fail;
				}
				writeFile.open(DB_NAME + ".dat", ios::trunc);
				writeFile.close();
				return 1;
			}
			//DELETE * FORM DB_NAME WHERE......
			readFile.open(DB_NAME + ".dat", ios::_Nocreate);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.open(DB_NAME + ".dat", ios::_Nocreate | ios::in | ios::out | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			T temp;
			for (int count = 0; readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp)); count++) {
				//去除id为-1的无效数据
				if (temp.id != -1) {
					int flag = 1;
					//遍历选择条件
					for (int i = 1; i < VALUES.size(); i++) {
						if (temp.getElemType(VALUES[i]) == "int") {
							if (temp.getIntElemByName(VALUES[i]) != entity.getIntElemByName(VALUES[i])) {
								flag = 0;
								break;
							}
						}
						else if (temp.getElemType(VALUES[i]) == "float") {
							if (temp.getFloatElemByName(VALUES[i]) != entity.getFloatElemByName(VALUES[i])) {
								flag = 0;
								break;
							}
						}
						else {
							if (strcmp(temp.getCharElemByName(VALUES[i]), entity.getCharElemByName(VALUES[i]))) {
								flag = 0;
								break;
							}
						}
					}
					//符合条件，删除
					if (flag) {
						res++;
						//移动文件指针，在count * sizeof(entity)出重写数据，将id置为-1
						writeFile.seekp(count * sizeof(entity), ios::beg);
						T deleteT;
						deleteT.setId(-1);
						writeFile.write((char*)&deleteT, sizeof(deleteT));
					}
				}
			}
			readFile.close();
			writeFile.close();
			return res;
		}//捕获文件操作异常，返回-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}//捕获字段赋值异常，返回-1
		catch (const char* e) {
			cout << "exception: " << e << endl;
			return -1;
		}
	}

	//更新数据，DB_NAME为表名，Sentity为选择条件，Uentity为更新内容，VALUES为选择字段
	//操作成功，返回受影响的行数，操作失败，返回-1
	template<typename T>
	static int update(string DB_NAME, T& Sentity, T& Uentity, vector<string>& VALUES)
	{
		ofstream writeFile;
		ifstream readFile;
		try {
			readFile.open(DB_NAME + ".dat", ios::_Nocreate);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.open(DB_NAME + ".dat", ios::_Nocreate | ios::in | ios::out | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			T temp;
			for (int count = 0; readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp)); count++) {
				//去除id为-1的无效数据
				if (temp.id != -1) {
					int flag = 1;
					//遍历选择条件
					for (int i = 1; i < VALUES.size(); i++) {
						if (temp.getElemType(VALUES[i]) == "int") {
							if (temp.getIntElemByName(VALUES[i]) != Sentity.getIntElemByName(VALUES[i])) {
								flag = 0;
								break;
							}
						}
						else if (temp.getElemType(VALUES[i]) == "float") {
							if (temp.getFloatElemByName(VALUES[i]) != Sentity.getFloatElemByName(VALUES[i])) {
								flag = 0;
								break;
							}
						}
						else {
							if (strcmp(temp.getCharElemByName(VALUES[i]), Sentity.getCharElemByName(VALUES[i]))) {
								flag = 0;
								break;
							}
						}
					}
					//符合条件，更新
					if (flag) {
						//设置id不变
						Uentity.setId(temp.id);
						//移动文件指针，在count * sizeof(Sentity)出重写数据
						writeFile.seekp(count * sizeof(Sentity), ios::beg);
						writeFile.write((char*)&Uentity, sizeof(Uentity));
					}
				}
			}
			readFile.close();
			writeFile.close();
		}//捕获文件操作异常，返回-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}//捕获字段赋值异常，返回-1
		catch (const char* e) {
			cout << "exception: " << e << endl;
			return -1;
		}
	}
};
