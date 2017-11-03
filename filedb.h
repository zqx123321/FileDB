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
#include<regex>
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
			int TSize = sizeof(temp);
			readFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			//SELECT * FORM DB_NAME
			if (VALUES[0] == "all") {
				while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
					//ȥ��idΪ-1����Ч����
					if (!temp.dirty)
						resultSet.push_back(temp);
				}
			}
			//SELECT * FORM DB_NAME WHERE......
			else {
				if (VALUES.size() == 2) {
					//id����
					if (VALUES[1] == "id") {
						selectId(readFile, entity.getId(), resultSet);
					}
					//������ѯ��������
					else {
						if (temp.getElemType(VALUES[1]) == "int")
							selectInt(readFile, VALUES[1], entity.getIntElemByName(VALUES[1]), resultSet);
						else if (temp.getElemType(VALUES[1]) == "float")
							selectFloat(readFile, VALUES[1], entity.getFloatElemByName(VALUES[1]), resultSet);
						else
							selectChar(readFile, VALUES[1], entity.getCharElemByName(VALUES[1]), resultSet);
					}
				}
				//�����ѯ����
				else {
					selectMany(readFile, entity, VALUES, resultSet);
				}
			
			}
			readFile.close();
			return resultSet.size();
		}//�����ļ������쳣������-1
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}
	}

	template<typename T>
	static void selectInt(ifstream& readFile, string valueName,int value, vector<T>& resultSet) {
		T temp;
		int TSize = sizeof(T);
		while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
			if (!temp.dirty && temp.getIntElemByName(valueName) == value) {
				resultSet.push_back(temp);
			}
		}
	}

	template<typename T>
	static void selectChar(ifstream& readFile, string valueName, char* value, vector<T>& resultSet) {
		T temp;
		int TSize = sizeof(T);
		while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
			if (!temp.dirty && !strcmp(temp.getCharElemByName(valueName),value)) {
				resultSet.push_back(temp);
			}
		}
	}

	template<typename T>
	static void selectFloat(ifstream& readFile, string valueName, float value, vector<T>& resultSet) {
		T temp;
		int TSize = sizeof(T);
		while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
			if (!temp.dirty && temp.getFloatElemByName(valueName) == value) {
				resultSet.push_back(temp);
			}
		}
	}

	template<typename T>
	static void selectId(ifstream& readFile, int Id, vector<T>& resultSet) {
		T temp;
		readFile.seekg(Id * sizeof(temp), ios::beg);
		readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp));
		resultSet.push_back(temp);
	}

	//�ɷ�ʵ�ּ��仯?���ԣ�
	template<typename T>
	static void selectMany(ifstream& readFile, T& entity, vector<string>& VALUES, vector<T>& resultSet) {
		T temp;
		int TSize = sizeof(T);
		//���仯����
		vector<string>elemType;
		vector<int>elemIntValue;
		vector<float>elemFloatValue;
		vector<char*>elemCharValue;
		elemType.push_back("");
		elemIntValue.push_back(0);
		elemFloatValue.push_back(0);
		elemCharValue.push_back("");
		for (int i = 1; i < VALUES.size(); i++) {
			elemType.push_back(entity.getElemType(VALUES[i]));
		}
		for (int i = 1; i < VALUES.size(); i++) {
			if (elemType[i] == "int") {
				elemIntValue.push_back(entity.getIntElemByName(VALUES[i]));
				elemFloatValue.push_back(0);
				elemCharValue.push_back("");
			}
			else if (elemType[i] == "float") {
				elemFloatValue.push_back(entity.getFloatElemByName(VALUES[i]));
				elemIntValue.push_back(0);
				elemCharValue.push_back("");
			}
			else {
				elemFloatValue.push_back(0);
				elemIntValue.push_back(0);
				elemCharValue.push_back(entity.getCharElemByName(VALUES[i]));
			}
		}

		while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
			if (!temp.dirty) {
				int flag = 1;
				//����ѡ������
				for (int i = 1; i < VALUES.size(); i++) {
					if (elemType[i] == "int") {
						if (temp.getIntElemByName(VALUES[i]) != elemIntValue[i]) {
							flag = 0;
							break;
						}
					}
					else if (elemType[i] == "float") {
						if (temp.getFloatElemByName(VALUES[i]) != elemFloatValue[i]) {
							flag = 0;
							break;
						}
					}
					else {
						if (strcmp(temp.getCharElemByName(VALUES[i]), elemCharValue[i])) {
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
	}

	//�ַ�����ģ����ѯ�������Ӵ���ģ����ѯ
	template<typename T>
	static int selectLike(string DB_NAME, string valueName,char* value, vector<T>& resultSet) {
		ifstream readFile;
		try {
			T temp;
			int TSize = sizeof(temp);
			readFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
				if (!temp.dirty != -1 && strstr(temp.getCharElemByName(valueName), value)) {
					resultSet.push_back(temp);
				}
			}
			return resultSet.size();
		}
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}

	}
	

	//�ַ�����ģ����ѯ������������ʽ��ģ����ѯ
	template<typename T>
	static int selectRegex(string DB_NAME, string valueName, regex& rx, vector<T>& resultSet) {
		ifstream readFile;
		try {
			T temp;
			int TSize = sizeof(temp);
			readFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			while (readFile.read(reinterpret_cast<char*>(&temp), TSize)) {
				if (!temp.dirty) {
					const char * value = temp.getCharElemByName(valueName);
					cmatch narrowMatch;
					if (regex_match(value, value+strlen(value), narrowMatch, rx) == 1)
						resultSet.push_back(temp);
				}
			}
			return resultSet.size();
		}
		catch (ios_base::failure &fail) {
			cout << fail.what() << endl;
			return -1;
		}

	}

	//�������ݣ�DB_NAMEΪ������entityΪ������ʵ��
	//�����ɹ���������Ӱ�������������ʧ�ܣ�����-1
	template<typename T>
	static int insert(string DB_NAME, vector<T>& entity)
	{
		int id;
		ofstream writeFile;
		ifstream readFile;
		try {
			//��ȡinfo�ļ�����ȡ���һ������id��ʵ��PK��������
			readFile.open(DB_NAME + "Info.dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.open(DB_NAME + ".dat", ios::app | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			
			readFile.read((char*)&id, sizeof(id));
			readFile.close();

			for (int i = 0; i < entity.size(); i++) {
				entity[i].setId(id);
				entity[i].setDirty(0);
				writeFile.write((char*)&entity[i], sizeof(entity[i]));
				id++;
			}

			writeFile.close();

			writeFile.open(DB_NAME + "Info.dat", ios::in | ios::out | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			writeFile.write((char*)&id, sizeof(id));
			writeFile.close();

			return entity.size();

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
				writeFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
				if (!writeFile) {
					ios_base::failure fail("ERROR");
					throw fail;
				}
				writeFile.open(DB_NAME + ".dat", ios::trunc);
				writeFile.close();
				return 1;
			}
			
			writeFile.open(DB_NAME + ".dat",  ios::in | ios::out | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			//id����
			if (VALUES[1] == "id") {
				T deleteT;
				deleteT.setDirty(1);
				writeFile.seekp(entity.getId() * sizeof(deleteT), ios::beg);
				writeFile.write((char*)&deleteT, sizeof(deleteT));
				return 1;
			}

			//DELETE * FORM DB_NAME WHERE......
			readFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			T temp;
			int TSize = sizeof(temp);

			//���仯����
			vector<string>elemType;
			vector<int>elemIntValue;
			vector<float>elemFloatValue;
			vector<char*>elemCharValue;
			elemType.push_back("");
			elemIntValue.push_back(0);
			elemFloatValue.push_back(0);
			elemCharValue.push_back("");
			for (int i = 1; i < VALUES.size(); i++) {
				elemType.push_back(entity.getElemType(VALUES[i]));
			}
			for (int i = 1; i < VALUES.size(); i++) {
				if (elemType[i] == "int") {
					elemIntValue.push_back(entity.getIntElemByName(VALUES[i]));
					elemFloatValue.push_back(0);
					elemCharValue.push_back("");
				}
				else if (elemType[i] == "float") {
					elemFloatValue.push_back(entity.getFloatElemByName(VALUES[i]));
					elemIntValue.push_back(0);
					elemCharValue.push_back("");
				}
				else {
					elemFloatValue.push_back(0);
					elemIntValue.push_back(0);
					elemCharValue.push_back(entity.getCharElemByName(VALUES[i]));
				}
			}

			for (int count = 0; readFile.read(reinterpret_cast<char*>(&temp), TSize); count++) {
				//ȥ��idΪ-1����Ч����
				if (!temp.dirty) {
					int flag = 1;
					//����ѡ������
					for (int i = 1; i < VALUES.size(); i++) {
						if (elemType[i] == "int") {
							if (temp.getIntElemByName(VALUES[i]) != elemIntValue[i]) {
								flag = 0;
								break;
							}
						}
						else if (elemType[i] == "float") {
							if (temp.getFloatElemByName(VALUES[i]) != elemFloatValue[i]) {
								flag = 0;
								break;
							}
						}
						else {
							if (strcmp(temp.getCharElemByName(VALUES[i]), elemCharValue[i])) {
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
						deleteT.setDirty(1);
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
			writeFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!writeFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			//id����
			if (VALUES[1] == "id") {
				Uentity.setId(Sentity.getId());
				writeFile.seekp(Sentity.getId() * sizeof(Sentity), ios::beg);
				writeFile.write((char*)&Uentity, sizeof(Uentity));
				return 1;
			}
			readFile.open(DB_NAME + ".dat", ios::in | ios::out | ios::binary);
			if (!readFile) {
				ios_base::failure fail("ERROR");
				throw fail;
			}
			T temp;

			//���仯����
			vector<string>elemType;
			vector<int>elemIntValue;
			vector<float>elemFloatValue;
			vector<char*>elemCharValue;
			elemType.push_back("");
			elemIntValue.push_back(0);
			elemFloatValue.push_back(0);
			elemCharValue.push_back("");
			for (int i = 1; i < VALUES.size(); i++) {
				elemType.push_back(Sentity.getElemType(VALUES[i]));
			}
			for (int i = 1; i < VALUES.size(); i++) {
				if (elemType[i] == "int") {
					elemIntValue.push_back(Sentity.getIntElemByName(VALUES[i]));
					elemFloatValue.push_back(0);
					elemCharValue.push_back("");
				}
				else if (elemType[i] == "float") {
					elemFloatValue.push_back(Sentity.getFloatElemByName(VALUES[i]));
					elemIntValue.push_back(0);
					elemCharValue.push_back("");
				}
				else {
					elemFloatValue.push_back(0);
					elemIntValue.push_back(0);
					elemCharValue.push_back(Sentity.getCharElemByName(VALUES[i]));
				}
			}

			for (int count = 0; readFile.read(reinterpret_cast<char*>(&temp), sizeof(temp)); count++) {
				//ȥ��idΪ-1����Ч����
				if (!temp.dirty) {
					int flag = 1;
					//����ѡ������
					for (int i = 1; i < VALUES.size(); i++) {
						if (elemType[i] == "int") {
							if (temp.getIntElemByName(VALUES[i]) != elemIntValue[i]) {
								flag = 0;
								break;
							}
						}
						else if (elemType[i] == "float") {
							if (temp.getFloatElemByName(VALUES[i]) != elemFloatValue[i]) {
								flag = 0;
								break;
							}
						}
						else {
							if (strcmp(temp.getCharElemByName(VALUES[i]), elemCharValue[i])) {
								flag = 0;
								break;
							}
						}
					}
					//��������������
					if (flag) {
						//����id����
						Uentity.setId(temp.id);
						Uentity.setDirty(0);
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
