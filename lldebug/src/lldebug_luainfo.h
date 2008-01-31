/*
 * Copyright (c) 2005-2008  cielacanth <cielacanth AT s60.xrea.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __LLDEBUG_LUAINFO_H__
#define __LLDEBUG_LUAINFO_H__

namespace lldebug {

#ifndef LLDEBUG_FRAME
std::string LuaToString(lua_State *L, int idx);
#endif

/**
 * @brief
 */
enum VarRootType {
	VARROOT_GLOBAL,
	VARROOT_LOCAL,
	VARROOT_REGISTRY,
	VARROOT_ENVIRON,
	VARROOT_STACK,
};

/**
 * @brief 異なるアプリケーション間でlua_Stateの値を共有するために必要なクラスです。
 */
class LuaHandle {
public:
	explicit LuaHandle()
		: m_luaState(0) {
	}

	LuaHandle(const LuaHandle &x)
		: m_luaState(x.m_luaState) {
	}

	~LuaHandle() {
	}

	LuaHandle &operator =(const LuaHandle &x) {
		m_luaState = x.m_luaState;
		return *this;
	}

#ifndef LLDEBUG_FRAME
	LuaHandle(lua_State *L) {
		SetState(L);
	}

	LuaHandle &operator =(lua_State *x) {
		SetState(x);
		return *this;
	}

	lua_State *oprator() const {
		return GetState();
	}

	lua_State *GetState() const {
		return reinterpret_cast<lua_State *>(m_luaState);
	}

	void SetState(lua_State *L) {
		m_luaState = reinterpret_cast<boost::uint64_t>(L);
	}
#endif

	friend bool operator==(const LuaHandle &x, const LuaHandle &y) {
		return (x.m_luaState == y.m_luaState);
	}
	friend bool operator!=(const LuaHandle &x, const LuaHandle &y) {
		return !(x == y);
	}
	friend bool operator <(const LuaHandle &x, const LuaHandle &y) {
		return (x.m_luaState < y.m_luaState);
	}
	friend bool operator >(const LuaHandle &x, const LuaHandle &y) {
		return (y < x);
	}
	friend bool operator<=(const LuaHandle &x, const LuaHandle &y) {
		return !(x > y);
	}
	friend bool operator>=(const LuaHandle &x, const LuaHandle &y) {
		return !(x < y);
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int) {
		ar & LLDEBUG_MEMBER_NVP(luaState);
	}

private:
	boost::uint64_t m_luaState;
};

/**
 * @brief Stackframe object.
 */
class LuaStackFrame {
public:
	explicit LuaStackFrame(const LuaHandle &lua = LuaHandle(), int level = -1);
	~LuaStackFrame();

	/// Get the lua state object.
	LuaHandle &GetLua() {
		return m_lua;
	}

	/// Get the level of the local stack frame.
	int GetLevel() const {
		return m_level;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int) {
		ar & LLDEBUG_MEMBER_NVP(lua);
		ar & LLDEBUG_MEMBER_NVP(level);
	}

private:
	LuaHandle m_lua;
	int m_level;
};

/**
 * @brief luaの変数情報を保持します。
 */
class LuaVar {
public:
	explicit LuaVar();
	virtual ~LuaVar();

#ifndef LLDEBUG_FRAME
	LuaVar(const LuaHandle &lua, VarRootType type, int level = -1);
	LuaVar(shared_ptr<LuaVar> parent, const std::string &name, int valueIdx);
	LuaVar(shared_ptr<LuaVar> parent, int keyIdx, int valueIdx);
#endif
	
	/// このオブジェクトが有効かどうか取得します。
	bool IsOk() const {
		return (m_valueType >= 0);
	}

	/// この変数があるlua_State *を取得します。
	const LuaHandle &GetLua() const {
		return m_lua;
	}

	/// 基底のテーブルタイプを取得します。
	VarRootType GetRootType() const {
		return m_rootType;
	}

	/// 基底テーブルがローカルだったときのスタックフレームレベルを取得します。
	int GetLevel() const {
		return m_level;
	}

	/// 変数名orテーブルのキー名を取得します。
	const std::string &GetName() const {
		return m_name;
	}

	/// 変数の値を文字列で取得します。
	const std::string &GetValue() const {
		return m_value;
	}

	/// 変数の値型を取得します。
	int GetValueType() const {
		return m_valueType;
	}

	/// 変数の値型を文字列で取得します。
	std::string GetValueTypeName() const {
		return "";//lua_typename(NULL, m_valueType);
	}

	/// 変数がフィールドを持つかどうかを取得します。
	bool HasFields() const {
		return m_hasFields;
	}

	/// 変数がフィールドを持つかどうかを設定します。
	void SetHasFields(bool hasFields) {
		m_hasFields = hasFields;
	}

	/// 親を取得します。
	shared_ptr<LuaVar> GetParent() const {
		return m_parent;
	}

	friend bool operator==(const LuaVar &x, const LuaVar &y) {
		bool xhasp = (x.m_parent == NULL);
		bool yhasp = (y.m_parent == NULL);
		if (xhasp != yhasp) {
			return false;
		}

		if (x.m_lua != y.m_lua || x.m_name != y.m_name) {
			return false;
		}

		if (x.m_parent == NULL) {
			return (
				x.m_rootType == y.m_rootType &&
				x.m_level == y.m_level);
		}
		else {
			return (*x.m_parent == *y.m_parent);
		}
	}

	friend bool operator!=(const LuaVar &x, const LuaVar &y) {
		return !(x == y);
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int) {
		ar & LLDEBUG_MEMBER_NVP(lua);
		ar & LLDEBUG_MEMBER_NVP(rootType);
		ar & LLDEBUG_MEMBER_NVP(level);
		ar & LLDEBUG_MEMBER_NVP(parent);
		ar & LLDEBUG_MEMBER_NVP(name);
		ar & LLDEBUG_MEMBER_NVP(value);
		ar & LLDEBUG_MEMBER_NVP(valueType);
		ar & LLDEBUG_MEMBER_NVP(hasFields);
	}

private:
	LuaHandle m_lua;
	VarRootType m_rootType;
	int m_level;  ///< 基底がローカルだった場合のスタックフレームレベルです。
	shared_ptr<LuaVar> m_parent;

	std::string m_name;
	std::string m_value;
	int m_valueType;
	bool m_hasFields;
};


/**
 * @brief 関数の呼び出し履歴を保持します。
 */
class LuaBacktrace {
public:
	explicit LuaBacktrace();
	~LuaBacktrace();

#ifndef LLDEBUG_FRAME
	explicit LuaBacktrace(const LuaHandle &lua,
						  const std::string &name,
						  const std::string &sourceKey,
						  const std::string &sourceTitle,
						  int line, int level);
#endif

	/// このコールがなされたlua_Stateです。
	const LuaHandle &GetLua() const {
		return m_lua;
	}

	/// 呼び出し元の関数の名前です。
	const std::string &GetFuncName() const {
		return m_funcName;
	}

	/// このコールがなされたソースの識別子です。
	const std::string &GetKey() const {
		return m_key;
	}

	/// このコールがなされたソースのタイトルです。
	const std::string &GetTitle() const {
		return m_sourceTitle;
	}

	/// このコールがなされたソースの行番号です。
	int GetLine() const {
		return m_line;
	}

	/// 関数のスタックレベルです。
	int GetLevel() const {
		return m_level;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int) {
		ar & LLDEBUG_MEMBER_NVP(lua);
		ar & LLDEBUG_MEMBER_NVP(funcName);
		ar & LLDEBUG_MEMBER_NVP(key);
		ar & LLDEBUG_MEMBER_NVP(sourceTitle);
		ar & LLDEBUG_MEMBER_NVP(line);
		ar & LLDEBUG_MEMBER_NVP(level);
	}

public:
	LuaHandle m_lua;
	std::string m_funcName;
	std::string m_key;
	std::string m_sourceTitle;
	int m_line;
	int m_level;
};

typedef std::vector<LuaVar> LuaVarList;
typedef std::vector<LuaVar> LuaStackList;
typedef std::vector<LuaBacktrace> LuaBacktraceList;

}

#endif
