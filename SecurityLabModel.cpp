#include "SecurityLabModel.h"
#include <QFile>
#include <qlineedit.h>
#include <QChar>

void SetHeader(Headers i_header, QAbstractItemModel* i_model)
{
	QString title("");
	if (i_header == Headers::USERNAME)
		title = "Username";
	else if (i_header == Headers::PASSWORD)
		title = "Password";
	else if (i_header == Headers::ISBLOCKED)
		title = "Blocked";
	else if (i_header == Headers::PASSWORDVERIFICATION)
		title = "verification";

	int column = static_cast<int>(i_header);
	i_model->insertColumn(column);
	i_model->setHeaderData(column, Qt::Horizontal, title);

}

bool SecurityLabModel::_CheckPassword(QString password)
{
	bool lowercase_letters = false;
	bool uppercase_letters = false;
	bool numbers_exist = false;
	for (auto symbol : password)
	{
		if (symbol.isDigit()) {
			numbers_exist = true;
			continue;
		}
		if (symbol.isLower())
		{
			lowercase_letters = true;
			continue;
		}
		if (symbol.isUpper())
		{
			uppercase_letters = true;
		}
	}

	if (!lowercase_letters || !uppercase_letters || !numbers_exist)
	{
		return false;
	}
	return true;
}

Page SecurityLabModel::GetCurrentPage() const
{
	return m_current_page;
}

void SecurityLabModel::SetCurrentPage(Page i_page)
{
	if (m_current_page == i_page)
		return;
	m_current_page = i_page;

	emit PageChanged(m_current_page);

	SetLoginUsername("");
	SetLoginPassword("");
	if (i_page != Page::UserPage)
		mp_current_user = nullptr;
}

void SecurityLabModel::SetLoginUsername(QString username)
{
	m_login_parameters.m_username = username;
	emit LoginUsernameChanged(username);
}

void SecurityLabModel::SetLoginPassword(QString password)
{
	if (m_login_parameters.m_password == password)
		return;

	m_login_parameters.m_password = password;
	emit LoginPasswordChanged(password);
}

SecurityLabModel::SecurityLabModel()
{
	m_current_page = Page::LoginPage;
	SetHeader(Headers::USERNAME, this);
	SetHeader(Headers::PASSWORD, this);
	SetHeader(Headers::ISBLOCKED, this);
	SetHeader(Headers::PASSWORDVERIFICATION, this);
	ReadUsersFromFile();
	if (m_users_list.isEmpty())
	{
		User admin("ADMIN");
		_AddUser(admin);
		SetCurrentPage(Page::AdminPage);
	}
}

SecurityLabModel::~SecurityLabModel()
{
	WriteUsersToFile();
}

void SecurityLabModel::_FillRow(const int i_row_index, const User& i_user)
{
	QModelIndex insert_index = index(i_row_index, static_cast<int>(Headers::USERNAME));
	setData(insert_index, i_user.GetUserName());
	insert_index = index(i_row_index, static_cast<int>(Headers::PASSWORD));
	setData(insert_index, i_user.GetPassword());
	insert_index = index(i_row_index, static_cast<int>(Headers::ISBLOCKED));
	setData(insert_index, i_user.GetIsBlocked());
	insert_index = index(i_row_index, static_cast<int>(Headers::PASSWORDVERIFICATION));
	setData(insert_index, i_user.GetVerifyNeeded());
}

User* SecurityLabModel::GetAdmin()const
{
	return mp_admin;
}

bool SecurityLabModel::Login()
{
	if (m_login_parameters.m_username.isEmpty())
	{
		emit Message("Username cannot be empty");
		return false;
	}

	for (auto& user : m_users_list) {
		if (user.GetUserName() == m_login_parameters.m_username) {
			if (user.GetIsBlocked())
			{
				emit Message("user is blocked");
				return false;
			}
			if (user.GetPassword() == m_login_parameters.m_password)
			{
				if (&user == mp_admin)
					SetCurrentPage(Page::AdminPage);
				else
				{
					SetCurrentPage(Page::UserPage);
					mp_current_user = &user;
				}
				return true;
			}
			else
			{
				m_blocking_contenders[&user]++;
				if (m_blocking_contenders[&user] >= 3)
				{
					_BlockUser(&user);
					emit Message("number of password attempts exceeded, user : " + user.GetUserName() + " blocked");
					return false;
				}
				else
				{
					emit Message("wrong password, attempts left : " + QString::number(3 - m_blocking_contenders[&user]));
					return false;
				}

			}
		}
	}

	emit Message("user with this name does not exist");
	return false;


}

void SecurityLabModel::Logout()
{
	WriteUsersToFile();
	SetCurrentPage(Page::LoginPage);
}

void SecurityLabModel::SetIndex(const QModelIndex& index)
{
	if (m_selected_index == index)
		return;
	m_selected_index = index;
	emit IndexChanged(index.isValid());
}

bool SecurityLabModel::AddUser(QString username)
{
	if (username.toLower() == "admin") {
		emit Message("admin is already exist");
		return false;
	}
	if (username.isEmpty())
	{
		emit Message("Username is empty");
		return false;
	}
	User new_user(username);
	_AddUser(new_user);
	WriteUsersToFile();
}

void SecurityLabModel::_AddUser(User& i_user)
{
	m_users_list.push_front(i_user);
	if (i_user.GetUserName().toLower() == QString("ADMIN").toLower())
		mp_admin = &m_users_list.front();

	insertRow(0);
	_FillRow(0, i_user);
}


void SecurityLabModel::ResetPassword(QString old_password, QString new_password)
{
	User* p_current_user = nullptr;
	if (m_current_page == Page::AdminPage)
		p_current_user = mp_admin;
	else
		p_current_user = mp_current_user;

	if (p_current_user && p_current_user->GetPassword() == old_password)
	{
		if (p_current_user->GetVerifyNeeded())
		{
			if (_CheckPassword(new_password))
			{
				_ResetPassword(p_current_user, new_password);
				emit Message("password changed");
			}
			else
			{
				emit Message("password does not meet requirements [Az1-9]");
			}
		}
		else {
			_ResetPassword(p_current_user, new_password);
			emit Message("password changed");
		}
	}
	else
		emit Message("old password wrong");

}

void SecurityLabModel::_ResetPassword(User* user, QString new_password)
{
	int user_index = 0;

	for (auto& p_user : m_users_list)
	{
		if (&p_user == user)
		{
			user->SetPassword(new_password);
			QModelIndex section_index = index(user_index, static_cast<int>(Headers::PASSWORD));
			setData(section_index, user->GetPassword());
			WriteUsersToFile();
			return;
		}
	}
	user_index++;
}

void SecurityLabModel::BlockUser()
{
	if (m_current_page != Page::AdminPage)
		return;

	int selected_index = 0;
	bool isBlocked = false;
	for (auto it = m_users_list.begin(); it < m_users_list.end(); it++)
	{
		if (selected_index == m_selected_index.row())
		{
			isBlocked = !(*it).GetIsBlocked();
			(*it).SetBlocked(isBlocked);
		}
		selected_index++;
	}
	QModelIndex section_index = index(m_selected_index.row(), static_cast<int>(Headers::ISBLOCKED));
	setData(section_index, isBlocked);
	WriteUsersToFile();
}

void SecurityLabModel::_BlockUser(User* user)
{
	int user_index = 0;
	for (auto user_to_block : m_users_list)
	{
		if (&user_to_block == user)
		{
			QModelIndex section_index = index(m_selected_index.row(), static_cast<int>(Headers::ISBLOCKED));
			setData(section_index, true);
			WriteUsersToFile();
			break;
		}
		user_index++;
	}
}

void SecurityLabModel::DisableVerification()
{
	if (m_current_page != Page::AdminPage)
		return;

	int selected_index = 0;
	bool verification_needed = false;
	for (auto it = m_users_list.begin(); it < m_users_list.end(); it++)
	{
		if (selected_index == m_selected_index.row())
		{
			verification_needed = !(*it).GetVerifyNeeded();
			(*it).SetVerifyNeeded(verification_needed);
		}
		selected_index++;
	}
	QModelIndex section_index = index(m_selected_index.row(), static_cast<int>(Headers::PASSWORDVERIFICATION));
	setData(section_index, verification_needed);
	WriteUsersToFile();
}

void SecurityLabModel::WriteUsersToFile() const
{
	QFile users_storage("users_storage");
	if (users_storage.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QDataStream save(&users_storage);
		save.setVersion(QDataStream::Qt_6_1);

		for (const auto& user : m_users_list) {
			save << user;
		}

		users_storage.close();
	}
}

void SecurityLabModel::ReadUsersFromFile()
{
	QFile users_storage("users_storage");
	if (users_storage.open(QIODevice::ReadOnly))
	{
		QDataStream load(&users_storage);
		load.setVersion(QDataStream::Qt_6_1);

		while (!load.atEnd())
		{
			User user;
			load >> user;
			_AddUser(user);
		}
		users_storage.close();
	}
}