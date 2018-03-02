將學期初公告Nany Lynch (MIT)影片寫成c++14 pseudocode。
原先是考慮先傳QUERY給children然後當所有children回傳wait & (ACCEPT or REJECT)再發送go 給children，才進行下一個round，但是一樣是時間會被傳送較慢的拉住。
所以採隨時更新的策略，設計概念如同flood經過不同坡度地形，越陡的會先經過。直到所有QUERY完之後，leaf會回傳結束訊號TERM，直到root接到結束，root會是第一位知道整個algo完成，再往子樹傳送完成的訊號，其中子樹往root回傳以0當旗標，root往leaves傳送以1當旗標。
因為未實作數種operator，用”\” 代表差集、用” ∪” 代表聯集、Set指派用”=”作替代，另外此實作還有一個缺點，理論上send 和 receive要 atomic ，不過在此寫法不可能，因為是遞迴呼叫。

Time Complexity : O(n^3)

#ifndef PROCESS_HPP
#define PROCESS_HPP
#include <set>
#include <algorithm>
#define MAX_DISTANCE 99999

class Process;
using Set = std::set<Process*>;//pointers to other process
enum class FLAG{QUERY,REJECT,ACCEPT,TERM,};

class Process{
// each process need to set Children and pid first
// then run AsyncBFSTree()
//root need run setRoot()
public:
	Process(){  /* do nothing*/}
	~Process(){/* do nothing*/}
	void setRoot(){this->isRoot = true;}
	void setChildren(Set Neighbors) {Children = Neighbors;}
Set getChildren() { return Children \ Father; }
	void setpid(int pid) {this->pid = pid;}
int getpid(){return this->pid; }
	int getDistance() { return this->d; }
	void AsyncBFSTree();
	void receive(Set Source, FLAG FLAG, int n);

private:
	int pid;
	int d = MAX_DISTANCE;//distance to root
	bool finished = false;
	bool founded = false;;
	bool isRoot = false;
	Set Father = {};//father set
	Set Children = {};//default neighbor
	Set EndCheck = {};//count for terminate
	void send(Set Target, FLAG FLAG, int n);
};

void Process::AsyncBFSTree() {
	//Algorithm runs from here
	if (isRoot) {
		Father.emplace(this); //just root
		send(Children, FLAG::QUERY, 0);
	}
	while ((!finished) && founded) {
		if (Children.size() == 0) {
			//if there is no son, this is leaf
			send(Father, FLAG::TERM, 0);
		}else {
			//just keep querying until termination
			send(Children, FLAG::QUERY, d);
		}
	}
}

void Process::receive(Set Source, FLAG FLAG, int n) {
	switch (FLAG)
	{
	case FLAG::QUERY:
		if (n + 1 < d) {
			founded = true; //enter while loop
			Father = Source;
			d = n + 1;
			send(Source, FLAG::ACCEPT, 0);
			send(Children \ Father, FLAG::QUERY, d);
		}else { // d does not change
			send(Source, FLAG::REJECT, 0);
		}

	case FLAG::REJECT:
		Children = Children \ Source;
		EndCheck = EndCheck \ Source;

	case FLAG::ACCEPT:
		Children = Children ∪ Source;

	case FLAG::TERM:
		if (n == 0) {//n here like a flag
			EndCheck = EndCheck ∪ Source;
			if (EndCheck == Children \ Father){
				//all children sended TERM
				if (isRoot) {
					//all subtrees sended TERM
					send(Children, FLAG:: TERM, 1);
				}else{
					//keep sending TERM sign
					send(Father, FLAG:: TERM, 0);
				}
			}else {// not all children term
				//keep waiting all children
			}
		}else {//n == 1
			finished = true; //leave while loop
			//keep sending finished sign
			send(Children \ Father, FLAG:: TERM, 1);
		}

	default:
		break;
	}
}

void Process::send(Set Target, FLAG FLAG, int n) {
	// implemented in other way is still ok, such as using named pipe
	//need Target start receive()
	const Set Self = { this };
	std::for_each(Target.begin, Target.end, [&Self,&n](Process* T) {
		T->receive(Self, FLAG::QUERY, n);
	});
}

#endif // !PROCESS_HPP
