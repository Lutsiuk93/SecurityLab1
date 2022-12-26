#pragma once
#include <QStandardItemModel>
#include <QObject>
#include "User.h"
#include <map>

enum class Page
{
	LoginPage = 0,
	AdminPage,
	UserPage
};

enum class Headers
{
	USERNAME,
	PASSWORD,
	ISBLOCKED,
	PASSWORDVERIFICATION
};

class SecurityLabModel : public QStandardItemModel
{
	Q_OBJECT
public:
	SecurityLabModel();
	~SecurityLabModel();
public slots:
	Page GetCurrentPage() const;
	void SetCurrentPage(Page i_page);
	bool AddUser(QString username);
	void ReadUsersFromFile();
	void WriteUsersToFile() const;
	void ResetPassword(QString old_password, QString new_password);
	bool Login();
	User* GetAdmin() const;
	void BlockUser();
	void DisableVerification();

	void Logout();
	void SetLoginUsername(QString username);
	void SetLoginPassword(QString username);
	void SetIndex(const QModelIndex& index);

signals:
	void Message(QString o_error_message);
	void LoginUsernameChanged(QString username);
	void LoginPasswordChanged(QString password);
	void PageChanged(Page page);
	void IndexChanged(bool o_is_valid);

private:
	void _FillRow(const int i_row_index, const User& i_user);
	bool _CheckPassword(QString i_password);
	void _AddUser(User& user);
	void _BlockUser(User* user);
	void _ResetPassword(User* user, QString new_password);
private:
	QList<User> m_users_list;
	User* mp_admin = nullptr;
	User* mp_current_user = nullptr;
	Page m_current_page;
	struct LoginParameters
	{
		QString m_username;
		QString m_password;
	}m_login_parameters;

	std::map<User*, int> m_blocking_contenders;
	QModelIndex m_selected_index;
};

