// Minesweeper.cpp : 定义应用程序的入口点。
//

#include "Minesweeper.h"

#include <random>
#include <fstream>

using namespace std;


static const wstring ImgPath = L"./imgs/";

static std::fstream g_fsLog("./dingfang.log", std::ios::app);

#define LOG()  { g_fsLog << __FUNCTION__ << "(" << __LINE__ << "): "

#define LOG_END    "\n"; \
                   g_fsLog.flush(); \
                   }


Minesweeper::Minesweeper()
{
	LOG() << "hello world" << LOG_END;
}


Minesweeper::~Minesweeper()
{
}

std::wstring Minesweeper::GetSkinFolder()
{
	return L"minesweeper";
}

std::wstring Minesweeper::GetSkinFile()
{
	return L"minesweeper.xml";
}

std::wstring Minesweeper::GetWindowClassName() const
{
	return L"Minesweeper";
}

void Minesweeper::InitWindow()
{
	InitMenu();
	InitCounter();
	InitFaceBtn();
	CreateCubeHBox();
}

void Minesweeper::InitMenu()
{
	m_primaryBtn = dynamic_cast<ui::Button *>(FindControl(L"primary"));
	m_intermediateBtn = dynamic_cast<ui::Button *>(FindControl(L"intermediate"));
	m_advancedBtn = dynamic_cast<ui::Button *>(FindControl(L"advanced"));

	if (m_primaryBtn != nullptr)
	{
		m_primaryBtn->AttachClick(nbase::Bind(&Minesweeper::OnMenuButtonClick, this, std::placeholders::_1));
	}

	if (m_intermediateBtn != nullptr)
	{
		m_intermediateBtn->AttachClick(nbase::Bind(&Minesweeper::OnMenuButtonClick, this, std::placeholders::_1));
	}

	if (m_advancedBtn != nullptr)
	{
		m_advancedBtn->AttachClick(nbase::Bind(&Minesweeper::OnMenuButtonClick, this, std::placeholders::_1));
	}
}

void Minesweeper::InitCounter()
{
	ui::HBox *m_minesCounter = dynamic_cast<ui::HBox *>(FindControl(L"counter_hbox"));
	if (m_minesCounter == nullptr)
	{
		LOG() << "m_minesCounter is null" << LOG_END;
		return;
	}

	int minesCount = m_currMinesCount;
	int count = m_minesCounter->GetCount();
	vector<int> showCount(count, 0);
	while (minesCount && count-- >= 0)
	{
		showCount[count] = minesCount % 10;
		minesCount /= 10;
	}

	for (int i = 0; i < showCount.size(); ++i)
	{
		m_minesCounter->GetItemAt(i)->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + _T("mines_time_") + to_wstring(showCount[i]) + _T(".png"));
	}
}

void Minesweeper::InitFaceBtn()
{
	m_faceBtn = dynamic_cast<ui::Button *>(FindControl(_T("control_game")));
	if (m_faceBtn == nullptr)
	{
		LOG() << "m_facebtn is null" << LOG_END;
	}

	m_faceBtn->AttachClick(nbase::Bind(&Minesweeper::OnMenuButtonClick, this, std::placeholders::_1));
}

void Minesweeper::CreateCubeHBox()
{
	m_cubeVBox = dynamic_cast<ui::VBox *>(FindControl(_T("cube_vbox")));
	if (m_cubeVBox == nullptr)
	{
		return;
	}

	m_cubeVBox->RemoveAll();
	m_minesfield.clear();
	LOG() << "CreateCubHBox" << LOG_END;

	for (int i = 0; i < m_col; ++i)
	{
		vector<ui::Button *> mineLine;
		ui::HBox *cubeHBox = new ui::HBox();
		for (int j = 0; j < m_row; ++j)
		{
			ui::Button *cubeBtn = new ui::Button();
			if (cubeBtn == nullptr)
			{
				LOG() << "cubeBtn is null" << LOG_END;
				MessageBox(GetHWND(), _T("init failed!"), _T("init error!"), 0);
				exit(-1);
			}
			cubeBtn->AttachClick(nbase::Bind(&Minesweeper::OnMinesButtonClick, this, std::placeholders::_1));
			cubeBtn->AttachAllEvents(nbase::Bind(&Minesweeper::EventMessage, this, std::placeholders::_1));
			MinesData *pMinesData = new MinesData(j, i);
			if (pMinesData == nullptr)
			{
				LOG() << "pMinesData is null" << LOG_END;
				MessageBox(GetHWND(), _T("init failed!"), _T("init error!"), 0);
				exit(-1);
			}
			cubeBtn->SetUserDataBase(pMinesData);

			cubeHBox->Add(dynamic_cast<ui::Control *>(cubeBtn));
			cubeBtn->SetAttribute(_T("class"), _T("minesweeper_cube"));
			cubeBtn->SetVisible(true);
			mineLine.push_back(cubeBtn);
		}
		m_cubeVBox->Add(dynamic_cast<ui::Control *>(cubeHBox));
		m_minesfield.emplace_back(move(mineLine));
	}

	GenerateMines();

	// 40为一格的长宽, 30位标题栏高度
	//m_cubeVBox->SetPos({ 0, 0, m_row * 40, m_col * 40 });
	ui::UiRect rt{ 0, 0, m_row * 40, m_col * 40 + 30 };
	SetPos(rt, false, 0);
	CenterWindow();

	return;
}

bool Minesweeper::OnMenuButtonClick(ui::EventArgs *args)
{
	if (args == nullptr || args->pSender == nullptr)
	{
		LOG() << "args: " << args << ", pSender: " << args->pSender << LOG_END;
		//return;
	}

	LOG() << "OnMenuButtonClick" << LOG_END;
	ui::Button *pBtn = dynamic_cast<ui::Button *>(args->pSender);
	if (pBtn->GetName() == _T("primary"))
	{
		m_row = 9;
		m_col = 9;
		m_minesCount = 10;
	}
	else if (pBtn->GetName() == _T("intermediate"))
	{
		m_row = 16;
		m_col = 16;
		m_minesCount = 40;
	}
	else if (pBtn->GetName() == _T("advanced"))
	{
		m_row = 30;
		m_col = 16;
		m_minesCount = 99;
	}
	else if (pBtn->GetName() == _T("control_game"))
	{
		pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + _T("mines_ing_normal.png"));
		pBtn->SetStateImage(ui::ControlStateType::kControlStatePushed, ImgPath + _T("mines_ing_push.png"));
	}

	m_currMinesCount = m_minesCount;
	m_currOpenCount = 0;
	InitCounter();
	CreateCubeHBox();

	return true;
}

bool Minesweeper::OnMinesButtonClick(ui::EventArgs *args)
{
	if (args == nullptr || args->pSender == nullptr)
	{
		LOG() << "args: " << args << ", pSender: " << args->pSender << LOG_END;
		//return;
	}
	
	ui::Button *pBtn = dynamic_cast<ui::Button *>(args->pSender);
	MinesData *pMD = dynamic_cast<MinesData *>(pBtn->GetUserDataBase());
	if (pMD == nullptr)
	{
		LOG() << "pMD is null" << LOG_END;
		return true;
	}

	int state = pMD->GetState();
	// 已经点开 或 标记为问号 或 标记为雷 就不能再点了
	if (state == MINES_STATE_CLICKED || state == MINES_STATE_ASK || state == MINES_STATE_FLAG)
	{
		return true;
	}
	
	wstring imgPath = ImgPath;
	if (!pMD->IsMines())
	{
		imgPath += L"mines_number_" + to_wstring(OpenSpace(pMD->GetRow(), pMD->GetCol())) + L".png";
		LOG() << "xy: (" << pMD->GetRow() << "," << pMD->GetCol() << ")" << ", state: " << pMD->GetState() << LOG_END;
		pMD->SetState(MINES_STATE_CLICKED);
		pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, imgPath);
		pBtn->SetStateImage(ui::ControlStateType::kControlStateHot, L"");

		CheckWin();
		return true;
	}

	// boom
	imgPath += L"mines_red.png";
	GameOver(false);
	pMD->SetState(MINES_STATE_CLICKED);
	pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, imgPath);
	pBtn->SetStateImage(ui::ControlStateType::kControlStateHot, L"");

	return true;
}

bool Minesweeper::EventMessage(ui::EventArgs *args)
{
	if (args == nullptr || args->pSender == nullptr)
	{
		LOG() << "args: " << args << ", pSender: " << args->pSender << LOG_END;
		//return;
	}

	if (args->Type == ui::EventType::kEventMouseRightButtonUp)
	{
		ui::Button *pBtn = dynamic_cast<ui::Button *>(args->pSender);
		MinesData *pMD = dynamic_cast<MinesData *>(pBtn->GetUserDataBase());
		if (pMD == nullptr)
		{
			LOG() << "pMD is null" << LOG_END;
			return true;
		}
		int state = pMD->GetState();
		LOG() << "state: " << state << LOG_END;
		if (state == MINES_STATE_NONE && m_currMinesCount > 0)
		{
			pMD->SetState(MINES_STATE_FLAG);
			pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + L"mines_flag.png");
			pBtn->SetStateImage(ui::ControlStateType::kControlStateHot, L"");
			--m_currMinesCount;
			InitCounter();
		}
		else if (state == MINES_STATE_FLAG)
		{
			pMD->SetState(MINES_STATE_ASK);
			pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + L"mines_ask.png");
			pBtn->SetStateImage(ui::ControlStateType::kControlStateHot, L"");
			++m_currMinesCount;
			InitCounter();
		}
		else if (state == MINES_STATE_ASK)
		{
			pMD->SetState(MINES_STATE_NONE);
			pBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + L"mines_normal.png");
			pBtn->SetStateImage(ui::ControlStateType::kControlStateHot, L"mines_clicked.png");
		}
	}

	return true;
}

void Minesweeper::GenerateMines()
{
	vector<int> scope(m_row * m_col);
	int size = scope.size();
	for (int i = 0; i < size; ++i)
	{
		scope[i] = i;
	}

	std::default_random_engine random(time(NULL));
	for (int i = 1; i <= m_minesCount && i < size - 1; i++)
	{
		std::uniform_int_distribution<int> dis(0, size - i);
		int idx = dis(random);
		swap(scope[size - i], scope[idx]);
	}

	for (int i = size - 1; i >= 0 && m_minesCount >= size - i; --i)
	{
		MinesData *pMD = dynamic_cast<MinesData *>(m_minesfield[scope[i] / m_row][scope[i] % m_row]->GetUserDataBase());
		pMD->SetMines(true);
	}
}

int Minesweeper::CalculateMines(int row, int col)
{
	int count = 0;
	for (int i = col - 1; i <= col + 1; ++i)
	{
		for (int j = row - 1; j <= row + 1; ++j)
		{
			if (i < 0 || j < 0 || i >= m_col || j >= m_row)
			{
				continue;
			}
			MinesData *pMD = dynamic_cast<MinesData *>(m_minesfield[i][j]->GetUserDataBase());
			if (pMD->IsMines())
			{
				++count;
			}
		}
	}

	return count;
}

int Minesweeper::OpenSpace(int row, int col)
{
	int count = CalculateMines(row, col);
	if (count != 0)
	{
		++m_currOpenCount;
		return count;
	}

	for (int i = col - 1; i <= col + 1; ++i)
	{
		for (int j = row - 1; j <= row + 1; ++j)
		{
			if (i < 0 || j < 0 || i >= m_col || j >= m_row)
			{
				continue;
			}
			MinesData *pMD = dynamic_cast<MinesData *>(m_minesfield[i][j]->GetUserDataBase());
			if (pMD == nullptr)
			{
				LOG() << "pMD is null" << LOG_END;
				return count;
			}
			int state = pMD->GetState();
			if (state == MINES_STATE_CLICKED || state == MINES_STATE_ASK || state == MINES_STATE_FLAG)
			{
				continue;
			}

			++m_currOpenCount;
			pMD->SetState(MINES_STATE_CLICKED);
			int cnt = CalculateMines(j, i);
			m_minesfield[i][j]->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + _T("mines_number_") + to_wstring(cnt) + _T(".png"));
			m_minesfield[i][j]->SetStateImage(ui::ControlStateType::kControlStateHot, L"");
			if (cnt == 0)
			{
				OpenSpace(j, i);
			}
		}
	}

	return count;
}

void Minesweeper::CheckWin()
{
	if (m_currOpenCount == m_row * m_col - m_minesCount)
	{
		GameOver(true);
	}
}

void Minesweeper::GameOver(bool isWin)
{
	if (isWin)
	{
		// 胜利, 更换图标
		m_faceBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + _T("mines_win_normal.png"));
		m_faceBtn->SetStateImage(ui::ControlStateType::kControlStatePushed, ImgPath + _T("mines_win_push.png"));
		MessageBox(GetHWND(), _T("You Win"), _T("Game Over"), 0);
		return;
	}

	// 输了, 更换图标, 显示所有雷
	m_faceBtn->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + _T("mines_lose_normal.png"));
	m_faceBtn->SetStateImage(ui::ControlStateType::kControlStatePushed, ImgPath + _T("mines_lose_push.png"));
	for (int i = 0; i < m_col; ++i)
	{
		for (int j = 0; j < m_row; ++j)
		{
			MinesData *pMD = dynamic_cast<MinesData *>(m_minesfield[i][j]->GetUserDataBase());
			if (pMD->IsMines())
			{
				wstring img = _T("mines_error.png");
				if (pMD->GetState() == MINES_STATE_FLAG)
				{
					img = _T("mines.png");
				}
				pMD->SetState(MINES_STATE_CLICKED);
				m_minesfield[i][j]->SetStateImage(ui::ControlStateType::kControlStateNormal, ImgPath + img);
				m_minesfield[i][j]->SetStateImage(ui::ControlStateType::kControlStateHot, _T(""));
			}
		}
	}
}

LRESULT Minesweeper::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}