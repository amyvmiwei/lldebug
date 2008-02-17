/*
 */

#ifndef __LLDEBUG_REMOTEENGINE_H__
#define __LLDEBUG_REMOTEENGINE_H__

#include "sysinfo.h"
#include "luainfo.h"

namespace lldebug {
namespace net {

/**
 * @brief Type of the command using TCP connection.
 */
enum RemoteCommandType {
	REMOTECOMMANDTYPE_SUCCESSED,
	REMOTECOMMANDTYPE_FAILED,

	REMOTECOMMANDTYPE_START_CONNECTION,
	REMOTECOMMANDTYPE_END_CONNECTION,

	REMOTECOMMANDTYPE_CHANGED_STATE,
	REMOTECOMMANDTYPE_UPDATE_SOURCE,
	REMOTECOMMANDTYPE_FORCE_UPDATESOURCE,
	REMOTECOMMANDTYPE_ADDED_SOURCE,
	REMOTECOMMANDTYPE_SAVE_SOURCE,
	REMOTECOMMANDTYPE_SET_UPDATECOUNT,

	REMOTECOMMANDTYPE_SET_BREAKPOINT,
	REMOTECOMMANDTYPE_REMOVE_BREAKPOINT,
	REMOTECOMMANDTYPE_CHANGED_BREAKPOINTLIST,

	REMOTECOMMANDTYPE_BREAK,
	REMOTECOMMANDTYPE_RESUME,
	REMOTECOMMANDTYPE_STEPINTO,
	REMOTECOMMANDTYPE_STEPOVER,
	REMOTECOMMANDTYPE_STEPRETURN,
	REMOTECOMMANDTYPE_OUTPUT_LOG,
	REMOTECOMMANDTYPE_EVAL,

	REMOTECOMMANDTYPE_REQUEST_FIELDSVARLIST,
	REMOTECOMMANDTYPE_REQUEST_LOCALVARLIST,
	REMOTECOMMANDTYPE_REQUEST_GLOBALVARLIST,
	REMOTECOMMANDTYPE_REQUEST_REGISTRYVARLIST,
	REMOTECOMMANDTYPE_REQUEST_ENVIRONVARLIST,
	REMOTECOMMANDTYPE_REQUEST_EVALVARLIST,
	REMOTECOMMANDTYPE_REQUEST_STACKLIST,
	REMOTECOMMANDTYPE_REQUEST_BACKTRACE,

	REMOTECOMMANDTYPE_VALUE_STRING,
	REMOTECOMMANDTYPE_VALUE_VARLIST,
	REMOTECOMMANDTYPE_VALUE_BACKTRACELIST,
	REMOTECOMMANDTYPE_VALUE_BREAKPOINTLIST,
};

/**
 * @brief The header of the command using TCP connection.
 */
struct CommandHeader {
	RemoteCommandType type;
	boost::int32_t ctxId;
	boost::uint32_t commandId;
	boost::uint32_t dataSize;
};

/**
 * @brief Data type for command contents.
 */
class CommandData {
public:
	explicit CommandData();
	explicit CommandData(const std::vector<char> &data);
	~CommandData();

	/// Get the size of this data.
	std::vector<char>::size_type GetSize() const {
		return m_data.size();
	}

	/// Get the impl data of command.
	std::vector<char> &GetImplData() {
		return m_data;
	}

	/// Get the impl data of command.
	const std::vector<char> &GetImplData() const {
		return m_data;
	}

	/// Get string for debug.
	std::string ToString() const {
		if (m_data.empty()) {
			return std::string("");
		}
		else {
			return std::string(&*m_data.begin(), m_data.size());
		}
	}

public:
	void Get_ChangedState(bool &isBreak) const;
	void Set_ChangedState(bool isBreak);

	void Get_UpdateSource(std::string &key, int &line, int &updateCount) const;
	void Set_UpdateSource(const std::string &key, int line, int updateCount);

	void Get_AddedSource(Source &source) const;
	void Set_AddedSource(const Source &source);

	void Get_SaveSource(std::string &key, string_array &sources) const;
	void Set_SaveSource(const std::string &key, const string_array &sources);

	void Get_SetUpdateCount(int &updateCount) const;
	void Set_SetUpdateCount(int updateCount);

	void Get_SetBreakpoint(Breakpoint &bp) const;
	void Set_SetBreakpoint(const Breakpoint &bp);

	void Get_RemoveBreakpoint(Breakpoint &bp) const;
	void Set_RemoveBreakpoint(const Breakpoint &bp);

	void Get_ChangedBreakpointList(BreakpointList &bps) const;
	void Set_ChangedBreakpointList(const BreakpointList &bps);

	void Get_OutputLog(LogType &type, std::string &str, std::string &key, int &line) const;
	void Set_OutputLog(LogType type, const std::string &str, const std::string &key, int line);

	void Get_Eval(std::string &str, LuaStackFrame &stackFrame) const;
	void Set_Eval(const std::string &str, const LuaStackFrame &stackFrame);

	void Get_RequestFieldVarList(LuaVar &var) const;
	void Set_RequestFieldVarList(const LuaVar &var);

	void Get_RequestLocalVarList(LuaStackFrame &stackFrame) const;
	void Set_RequestLocalVarList(const LuaStackFrame &stackFrame);

	void Get_RequestEnvironVarList(LuaStackFrame &stackFrame) const;
	void Set_RequestEnvironVarList(const LuaStackFrame &stackFrame);

	void Get_RequestEvalVarList(string_array &array, LuaStackFrame &stackFrame) const;
	void Set_RequestEvalVarList(const string_array &array, const LuaStackFrame &stackFrame);

	void Get_ValueString(std::string &str) const;
	void Set_ValueString(const std::string &str);

	void Get_ValueVarList(LuaVarList &vars) const;
	void Set_ValueVarList(const LuaVarList &vars);

	void Get_ValueBacktraceList(LuaBacktraceList &backtraces) const;
	void Set_ValueBacktraceList(const LuaBacktraceList &backtraces);

private:
	std::vector<char> m_data;
};

class RemoteEngine;
class Command;
class SocketBase;

typedef
	boost::function1<void, const Command &>
	CommandCallback;
typedef
	boost::function2<void, const Command &, const std::string &>
	StringCallback;
typedef
	boost::function2<void, const Command &, const LuaVarList &>
	LuaVarListCallback;
typedef
	boost::function2<void, const Command &, const BreakpointList &>
	BreakpointListCallback;
typedef
	boost::function2<void, const Command &, const LuaBacktraceList &>
	BacktraceListCallback;

/**
 * @brief The command using TCP connection.
 */
class Command {
public:
	Command(const CommandHeader &header, const CommandData &data)
		: m_header(header), m_data(data) {
	}

	~Command() {
	}

	/// Get the type of this command.
	RemoteCommandType GetType() const {
		return m_header.type;
	}

	/// Get the context id.
	int GetCtxId() const {
		return m_header.ctxId;
	}

	/// Get the command id.
	boost::uint32_t GetCommandId() const {
		return m_header.commandId;
	}

	/// Get the size of this data.
	boost::uint32_t GetDataSize() const {
		return m_header.dataSize;
	}

	/// Get the header of this command.
	CommandHeader &GetHeader() {
		return m_header;
	}

	/// Get the const header of this command.
	const CommandHeader &GetHeader() const {
		return m_header;
	}

	/// Get the const data of the command.
	CommandData &GetData() {
		return m_data;
	}

	/// Get the data of the command.
	const CommandData &GetData() const {
		return m_data;
	}

	/// Get the const impl data of the command data.
	std::vector<char> &GetImplData() {
		return m_data.GetImplData();
	}

	/// Get the const impl data of the command data.
	const std::vector<char> &GetImplData() const {
		return m_data.GetImplData();
	}

	/// Is this a response command ?
	bool IsResponse() const {
		return (m_response != NULL);
	}

	/// Call response function.
	void CallResponse() {
		m_response(*this);
		m_response.clear();
	}

	/// Get string for debug.
	std::string ToString() const {
		return m_data.ToString();
	}

private:
	friend class SocketBase;
	friend class RemoteEngine;
	explicit Command() {
	}

	/// Resize the impl data.
	void ResizeData() {
		m_data.GetImplData().resize(m_header.dataSize);
	}

	/// Set the response callback.
	void SetResponse(const CommandCallback &response) {
		m_response = response;
	}

private:
	CommandHeader m_header;
	CommandData m_data;
	CommandCallback m_response;
};

/**
 * @brief Remote engine for debugger.
 */
class RemoteEngine {
public:
	explicit RemoteEngine();
	virtual ~RemoteEngine();

	/// Start the debuggee program (context).
	int StartContext(int portNum, int ctxId, int waitSeconds);

	/// Start the debugger program (frame).
	int StartFrame(const std::string &hostName, const std::string &portName,
				   int waitSeconds);

	/// Is the socket connected ?
	bool IsConnected();

	void SetReadCommandCallback(const CommandCallback &callback);

	void ResponseSuccessed(const Command &command);
	void ResponseFailed(const Command &command);

	void ChangedState(bool isBreak);
	void UpdateSource(const std::string &key, int line, int updateCount, const CommandCallback &response);
	void ForceUpdateSource();
	void AddedSource(const Source &source);
	void SaveSource(const std::string &key, const string_array &sources);
	void SetUpdateCount(int updateCount);

	void SetBreakpoint(const Breakpoint &bp);
	void RemoveBreakpoint(const Breakpoint &bp);
	void ChangedBreakpointList(const BreakpointList &bps);

	void Break();
	void Resume();
	void StepInto();
	void StepOver();
	void StepReturn();

	void OutputLog(LogType type, const std::string &str, const std::string &key, int line);
	void Eval(const std::string &str, const LuaStackFrame &stackFrame,
			  const StringCallback &callback);
	
	void RequestFieldsVarList(const LuaVar &var, const LuaVarListCallback &callback);
	void RequestLocalVarList(const LuaStackFrame &stackFrame, const LuaVarListCallback &callback);
	void RequestEnvironVarList(const LuaStackFrame &stackFrame, const LuaVarListCallback &callback);
	void RequestEvalVarList(const string_array &array, const LuaStackFrame &stackFrame, const LuaVarListCallback &callback);
	void RequestGlobalVarList(const LuaVarListCallback &callback);
	void RequestRegistryVarList(const LuaVarListCallback &callback);
	void RequestStackList(const LuaVarListCallback &callback);

	void ResponseString(const Command &command, const std::string &str);
	void ResponseVarList(const Command &command, const LuaVarList &vars);
	void ResponseBacktraceList(const Command &command, const LuaBacktraceList &backtraces);

	/// Get id of the Context object.
	int GetCtxId() {
		scoped_lock lock(m_mutex);
		return m_ctxId;
	}

private:
	bool IsThreadActive();
	void SetThreadActive(bool is);
	void StartThread();
	void StopThread();
	void DoStartConnection(int ctxId);
	void DoEndConnection();
	void SetCtxId(int ctxId);
	CommandHeader InitCommandHeader(RemoteCommandType type,
									size_t dataSize,
									int commandId = 0);
	void WriteCommand(RemoteCommandType type,
					  const CommandData &data);
	void WriteCommand(RemoteCommandType type,
					  const CommandData &data,
					  const CommandCallback &callback);
	void WriteResponse(const Command &readCommand,
					   RemoteCommandType type,
					   const CommandData &data);
	void HandleReadCommand(const Command &command);
	void ServiceThread();

	friend class SocketBase;

private:
	shared_ptr<thread> m_thread;
	bool m_isThreadActive;
	mutex m_mutex;
	condition m_ctxCond;

	boost::asio::io_service m_ioService;
	boost::shared_ptr<SocketBase> m_socket;
	int m_ctxId;
	boost::uint32_t m_commandIdCounter;
	CommandCallback m_readCommandCallback;

	struct WaitResponseCommand {
		CommandHeader header;
		CommandCallback response;
	};
	typedef std::list<WaitResponseCommand> WaitResponseCommandList;
	WaitResponseCommandList m_waitResponseCommandList;

	//typedef std::queue<Command> ReadCommandQueue;
	//ReadCommandQueue m_readCommandQueue; // commands that were read.
};

/**
 * @brief Waiting for the boolean response command (success or failed).
 * 
 * This class must share all fields.
 */
struct BooleanCallbackWaiter {
	explicit BooleanCallbackWaiter();
	~BooleanCallbackWaiter();

	/// This method is called from RemoteEngine.
	void operator()(const Command &command);

	/// Wait reponse.
	bool Wait();

private:
	struct Impl;
	shared_ptr<Impl> impl; ///< shared object
};

} // end of namespace net
} // end of namespace lldebug

#endif