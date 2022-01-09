#pragma once

#include "Resource.h"
#include "framework.h"

#include <vector>

static constexpr int MINES_STATE_NONE    = 0;    // 还没有点击
static constexpr int MINES_STATE_CLICKED = 1;    // 点击过了
static constexpr int MINES_STATE_FLAG    = 2;    // 标记为雷
static constexpr int MINES_STATE_ASK     = 3;    // 标记问号

static constexpr int MINES = -1;

class MinesData : public ui::UserDataBase
{
public:
	MinesData(int row, int col) : m_row(row), m_col(col) {}
	int GetNumber() const { return m_number; };
	bool IsMines() const { return m_isMines; };
	void SetMines(bool isMines) { m_isMines = isMines; };
	void SetNumber(int number) { m_number = number; };
	int GetRow() const { return m_row; };
	int GetCol() const { return m_col; };
	int GetState() const { return m_state; };
	void SetState(int state) { m_state = state; };

private:
	int m_number{ MINES };
	bool m_isMines{ false };
	int m_row{ -1 };
	int m_col{ -1 };
	int m_state{ MINES_STATE_NONE };
};

class Minesweeper : public ui::WindowImplBase
{
public:
	Minesweeper();
	~Minesweeper();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;

	virtual void InitWindow() override;
	


	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void InitMenu();
	void InitCounter();
	void InitFaceBtn();
	void CreateCubeHBox();

	// 菜单按钮消息
	bool OnMenuButtonClick(ui::EventArgs *args);

	// 雷区方格按钮
	bool OnMinesButtonClick(ui::EventArgs *args);
	// 事件处理: 鼠标右键-->雷区方格右键标记
	bool EventMessage(ui::EventArgs *args);

	// 生成雷
	void GenerateMines();

	// 计算位置的周边雷数
	int CalculateMines(int row, int col);

	// 递归翻开空白
	int OpenSpace(int row, int col);

	void CheckWin();
	void GameOver(bool isWin);

private:
	ui::VBox *m_cubeVBox{ nullptr };
	ui::Button *m_primaryBtn{ nullptr };
	ui::Button *m_intermediateBtn{ nullptr };
	ui::Button *m_advancedBtn{ nullptr };

	ui::HBox *m_minesCounter{ nullptr };
	ui::Button *m_faceBtn{ nullptr };

	std::vector< std::vector<ui::Button *> > m_minesfield;
	int m_row{ 9 };
	int m_col{ 9 };
	int m_minesCount{ 10 };
	int m_currMinesCount{ 10 };

	int m_currOpenCount{ 0 };
};