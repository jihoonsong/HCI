#include "TouchHistory.h"

static TouchHistory* pTouchHistoryManager = nullptr;

TouchHistory::TouchHistory()
{
}


TouchHistory::~TouchHistory()
{
	// todo : clear the memory
}

TouchHistory* TouchHistory::GetInstance()
{
	if(pTouchHistoryManager == nullptr)
		pTouchHistoryManager = new TouchHistory;

	return pTouchHistoryManager;
}

void TouchHistory::PushEditHistory(EditorMode edit, int id)
{
	mEditHistory.push(std::pair<EditorMode, int>(edit, id));
}

std::pair<EditorMode, int> TouchHistory::GetLastEdit()
{
	return mEditHistory.top();
}

void TouchHistory::PopEditHistory()
{
	if(!mEditHistory.empty())
		mEditHistory.pop();
}

void TouchHistory::PushBackHistory(EditorMode edit, int id)
{
	mBackHistory.push(std::pair<EditorMode, int>(edit, id));
}

std::pair<EditorMode, int> TouchHistory::GetLastBack()
{
	return mBackHistory.top();
}

void TouchHistory::PopBackHistory()
{
	if(!mBackHistory.empty())
		mBackHistory.pop();
}

void TouchHistory::InsertPixelList(int id, Pixel pixel)
{
	std::unordered_map<int, std::vector<Pixel>*>::iterator itor;
	itor = mPixelList.find(id);
	if(itor == mPixelList.end())
	{
		std::vector<Pixel>* pixelVector = new std::vector < Pixel > ;
		pixelVector->push_back(pixel);

		mPixelList.insert(std::pair<int, std::vector<Pixel>*>(id, pixelVector));
	}
	else
	{
		std::vector<Pixel>* pixelVector = itor->second;
		pixelVector->push_back(pixel);
	}
}

std::vector<Pixel>* TouchHistory::GetPixelList(int id)
{
	std::unordered_map<int, std::vector<Pixel>*>::iterator itor;
	itor = mPixelList.find(id);
	if(itor == mPixelList.end())
		return nullptr;
	
	return itor->second;
}
