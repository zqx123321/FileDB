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
//���ݿ�CRUD����������װ
class FileDB
{
public:
	
	//ѡ�����ݣ�DB_NAMEΪ������entityΪѡ��������VALUESΪѡ���ֶΣ�resultSetΪ�����
	//�����ɹ���������Ӱ�������������ʧ�ܣ�����-1
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
					//ȥ��idΪ-1����Ч����
					if (temp.id != -1)
						resultSet.push_back(temp);
				}
				readFile.close();
			}
			//SELECT * FORM DB_NAME WHERE......
			else {
				while (readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
					//ȥ��idΪ-1����Ч����
					if (temp.id != -1) {
						int flag = 1;
						//����ѡ������
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
						//������������������
						if (flag)
							resultSet.push_back(temp);
					}
				}
				readFile.close();
			}
			return resultSet.size();
		}//�����ļ������쳣������-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}
	}

	//�������ݣ�DB_NAMEΪ������entityΪ������ʵ��
	//�����ɹ���������Ӱ�������������ʧ�ܣ�����-1
	template<typename T>
	static int insert(string DB_NAME, T& entity)
	{
		int id;
		ofstream writeFile;
		ifstream readFile;
		try {
			//��ȡinfo�ļ�����ȡ���һ������id��ʵ��PK��������
			readFile.open(DB_NAME + "Info.dat", ios::_Nocreate);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			readFile.read((char*)&id, sizeof(id));
			readFile.close();
			//PK����
			id++;
			//25��26��ʮ������0x1a��0x1b�ᴥ���ļ���βEOF(),��������
			if (id == 25) id += 2;
			entity.setId(id);
			//����PKд���ļ�
			writeFile.open(DB_NAME + "Info.dat", ios::_Nocreate);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.write((char*)&id, sizeof(id));
			writeFile.close();
			//���ݲ���
			writeFile.open(DB_NAME + ".dat", ios::app | ios::_Nocreate);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.write((char*)&entity, sizeof(entity));
			writeFile.close();
			return 1;
		}//�����ļ������쳣������-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}
	}

	//ɾ�����ݣ�DB_NAMEΪ������entityΪѡ��������VALUESΪѡ���ֶ�
	//�����ɹ���������Ӱ�������������ʧ�ܣ�����-1
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
				//ȥ��idΪ-1����Ч����
				if (temp.id != -1) {
					int flag = 1;
					//����ѡ������
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
					//����������ɾ��
					if (flag) {
						res++;
						//�ƶ��ļ�ָ�룬��count * sizeof(entity)����д���ݣ���id��Ϊ-1
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
		}//�����ļ������쳣������-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}//�����ֶθ�ֵ�쳣������-1
		catch (const char* e) {
			cout << "exception: " << e << endl;
			return -1;
		}
	}

	//�������ݣ�DB_NAMEΪ������SentityΪѡ��������UentityΪ�������ݣ�VALUESΪѡ���ֶ�
	//�����ɹ���������Ӱ�������������ʧ�ܣ�����-1
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
				//ȥ��idΪ-1����Ч����
				if (temp.id != -1) {
					int flag = 1;
					//����ѡ������
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
					//��������������
					if (flag) {
						//����id����
						Uentity.setId(temp.id);
						//�ƶ��ļ�ָ�룬��count * sizeof(Sentity)����д����
						writeFile.seekp(count * sizeof(Sentity), ios::beg);
						writeFile.write((char*)&Uentity, sizeof(Uentity));
					}
				}
			}
			readFile.close();
			writeFile.close();
		}//�����ļ������쳣������-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}//�����ֶθ�ֵ�쳣������-1
		catch (const char* e) {
			cout << "exception: " << e << endl;
			return -1;
		}
	}
};
