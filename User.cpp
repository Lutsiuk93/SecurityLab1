#include "User.h"
#include <qdatastream.h>

User::User(QString i_username, QString i_password, bool i_isBlocked, bool i_verification)
	: m_username(i_username), m_password(i_password), m_isBlocked(i_isBlocked), m_verify_needed(i_verification)
{
	true;
}

QString User::GetUserName() const
{
	return m_username;
}

QString User::GetPassword() const
{
	return m_password;
}

bool User::GetIsBlocked() const
{
	return m_isBlocked;
}

bool User::GetVerifyNeeded() const
{
	return m_verify_needed;
}

void User::SetUsername(QString i_username)
{
	m_username = i_username;
}

void User::SetPassword(QString i_password)
{
	m_password = i_password;
}

void User::SetBlocked(bool i_is_blocked)
{
	m_isBlocked = i_is_blocked;
}

void User::SetVerifyNeeded(bool i_verify_needed)
{
	m_verify_needed = i_verify_needed;
}

QDataStream& operator<< (QDataStream& out, const User& rhs)
{
	out << rhs.m_username << rhs.m_password << rhs.m_isBlocked << rhs.m_verify_needed;
	return out;
}

QDataStream& operator>> (QDataStream& in, User& rhs)
{
	in >> rhs.m_username >> rhs.m_password >> rhs.m_isBlocked >> rhs.m_verify_needed;
	return in;
}