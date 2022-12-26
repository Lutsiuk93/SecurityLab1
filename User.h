#pragma once
#include <QString>
class User
{
private:
	QString m_username;
	QString m_password;
	bool m_isBlocked;
	bool m_verify_needed;
public:
	User(QString username, QString password = {}, bool isBlocked = false, bool verification = false);
	User() = default;
	~User() = default;
	QString GetUserName() const;
	QString GetPassword() const;
	bool GetIsBlocked() const;
	bool GetVerifyNeeded() const;

	/* overload the operators */
	friend QDataStream& operator<< (QDataStream& out, const User& rhs);
	friend QDataStream& operator>> (QDataStream& in, User& rhs);

	void SetUsername(QString i_username);
	void SetPassword(QString i_password);
	void SetBlocked(bool i_is_blocked);
	void SetVerifyNeeded(bool i_verify_needed);
};

