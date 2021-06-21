#include <iostream>
#include <string>
#include <vector>
#include<unordered_map>
#include <ctime>
// #include <algorithm>
// #include <cassert>
// #include<fstream>
#include <random>

using namespace std;
#define MAX_SType 100   //服务器总类型数目
#define MAX_VMType 1000 //虚拟机总类型数目
#define MAX_BuySNum 100000 //购买服务器的总数量
#define MAX_VMNum 100000    //虚拟机的总数量
vector<string> outInfo;//总输出
vector<string> temVmOpInfo;//一天之中仅vm的操作结果
vector<string> temBuyS;//一天中购买服务器输出的总情况
unordered_map<int,int> temBuyTypeNum;//购买服务器类型ID，购买的类型台数

// 其中一天请求信息
vector<vector<string>> dayRequestInfos;
int allRequestDayNum=0;//总共的处理请求天数
//从所有虚拟机中筛选，得到最小规格的虚拟机

// 所有服务器的信息，服务器的种类最多100种
//服务器ID从0开始
int s_TypeNum;//服务器的总类型数
string s_Name[MAX_SType];
int s_A_CPU[MAX_SType];
int s_A_Mem[MAX_SType];
int s_B_CPU[MAX_SType];
int s_B_Mem[MAX_SType];
int s_HardCost[MAX_SType];
int s_PowerCost[MAX_SType];

//所有虚拟机的信息汇总，虚拟机的种类最多1000种
int VMTypeNum;//虚拟机的总类型数
string VM_Name[MAX_VMType];
int VM_CPU[MAX_VMType];
int VM_MEM[MAX_VMType];
int VM_isDouleNode[MAX_VMType];//单(0),双(1)

//**********************************实际情况*****************************************
//已购买服务器的数量
int allBuySNum=0;
//服务器实际，购买的服务器总量不能超过100000个
//服务器ID：服务器下标；mS_TypeID[i]:代表服务器类型的值
int mS_TypeID[MAX_BuySNum]={-1};//此服务器配置的类型ID
bool mS_isNoSpaceInstall[MAX_BuySNum]={false};//是否有已装满VM，无剩余空间，默认(fasle),装满(true),没装满(fasle)
int mS_VMNum[MAX_BuySNum]={0};//此服务器虚拟机的个数，若不为0则Run
int mS_A_CPU[MAX_BuySNum];//实时的剩余硬件资源
int mS_A_MEM[MAX_BuySNum];//实时的剩余硬件资源
int mS_B_CPU[MAX_BuySNum];//实时的剩余硬件资源
int mS_B_MEM[MAX_BuySNum];//实时的剩余硬件资源

//虚拟机的实际情况
bool isHaveVM[MAX_VMNum]={false};//代表虚拟机是否存在的标志位，有(ture),没有(false)，数据能否用需要先判断数据位
int  mVM_ID[MAX_VMNum]={-1};//虚拟机ID
int  mVM_TypeID[MAX_VMNum]={-1};//虚拟机的类型ID,从0开始，以虚拟机类型的下标为ID
int  mVM_inServerID[MAX_VMNum]={-1};//虚拟机所在服务器的ID，默认-1则表示没在服务器上
short mVM_NodePlace[MAX_VMNum]={-1};//虚拟机在服务器上的节点情况AB(0),A(1),B(2)，默认-1则表示不知道节点布置情况
//**********************************实际情况**********************************************************

//由输入得到所有可能的服务器类型参数
void setServerAllType(){
    bool res;
    scanf("%d",&s_TypeNum);
    string serverType,cpuCores,memorySize,serverCost,powerCost;//服务器：型号 , CPU 核数 , 内存大小 , 硬件成本 , 每日能耗成本
    for(int i =0;i<s_TypeNum;i++){
        cin>>serverType>>cpuCores>>memorySize>>serverCost>>powerCost;
        // generateServer(serverType,cpuCores,memorySize,serverCost,powerCost);//保存服务器的信息
        //保存服务器的型号
        for(int j =1;j<serverType.size() -1;j++){
            s_Name[i]+=serverType[j];
        }

        int _cpuCores =0,_memorySize=0,_serverCost=0,_powerCost=0;//其他参数初始化
        for(int j=0;j<cpuCores.size() -1;j++){
            _cpuCores = 10*_cpuCores + cpuCores[j] - '0';//cpuCores[i] - '0'当前识别到字符的数值
        }
        for(int j=0;j<memorySize.size() -1;j++){
            _memorySize = 10*_memorySize + memorySize[j] - '0';
        }
        for(int j=0;j<serverCost.size() -1;j++){
            _serverCost = 10*_serverCost + serverCost[j] - '0';
        }
        for(int j=0;j<powerCost.size()-1;j++){
            _powerCost = 10*_powerCost + powerCost[j] - '0';
        }
        //保存服务器信息
        s_A_CPU[i]=_cpuCores/2 ;
        s_A_Mem[i]=_memorySize/2;
        s_B_CPU[i]=_cpuCores/2 ;
        s_B_Mem[i]=_memorySize/2;
        s_HardCost[i]=_serverCost;
        s_PowerCost[i]=_powerCost;
    }
}

//由输入得到所有可能的虚拟机类型参数
void setVmAllType(){
    scanf("%d",&VMTypeNum);
    string vmType,vmCpuCores,vmMemory,vmTwoNodes;//虚拟机:型号 , CPU 核数 , 内存大小 , 是否双节点部署(0单节点；1双节点)
    for(int i =0;i<VMTypeNum;i++){
        cin>>vmType>>vmCpuCores>>vmMemory>>vmTwoNodes;
        string _vmType="";
        for(int j=1;j<vmType.size() -1;j++){
            _vmType += vmType[j];
        }
        int _vmCpuCores = 0,_vmMemory=0;
        for(int j=0;j<vmCpuCores.size()-1;j++){
            _vmCpuCores = _vmCpuCores*10 + vmCpuCores[j] - '0';
        }
        for(int j=0;j<vmMemory.size()-1;j++){
            _vmMemory = _vmMemory*10 + vmMemory[j] - '0';
        }
        if(vmTwoNodes[0] == '1'){
            VM_isDouleNode[i] = 1;
        }
        else{
            VM_isDouleNode[i] =0;
        }
        VM_Name[i]=_vmType;
        VM_CPU[i]=_vmCpuCores;
        VM_MEM[i]=_vmMemory;
    }
}

// 解析用户添加请求
void generateRequest(string &op,string &reqVmType,string &reqId){
    string _op,_reqVmType,_VmId;
    _op = op.substr(1,op.size() -2);//提取子字符串，0位是(
    _reqVmType = reqVmType.substr(0,reqVmType.size() -1);
    _VmId = reqId.substr(0,reqId.size() -1);
    dayRequestInfos.push_back(vector<string>{_op,_reqVmType,_VmId});
}

// 解析用户删除请求
void generateRequest(string &op,string &reqId){
    string _op,_VmId;
    _op = op.substr(1,op.size() -2);
    _VmId = reqId.substr(0,reqId.size() -1);
    dayRequestInfos.push_back(vector<string>{_op,_VmId});
}

//按照给定的虚拟机名字得到虚拟机类型ID
int getVMTypeID(string &mVMStr){
    for(int i=0;i<VMTypeNum;i++){
        if(mVMStr==VM_Name[i]){
            return i;
        }
    }
    return -1;
}

//按照给定的服务器名字得到服务器的类型ID
int getSTypeID(string &mSName){
    for(int i=0;i<s_TypeNum;i++){
        if(mSName==s_Name[i]){
            return i;
        }
    }
    return -1;
}

//服务器初始化,用后者的服务器类型ID初始化指定服务器
void initServer(int _mInitSID, int _mTypeSID){
    mS_TypeID[_mInitSID]=_mTypeSID;//此服务器的类型ID
    mS_isNoSpaceInstall[_mInitSID]=false;
    mS_VMNum[_mInitSID]=0;//此服务器虚拟机的个数
    mS_A_CPU[_mInitSID]=s_A_CPU[_mTypeSID];
    mS_A_MEM[_mInitSID]=s_A_Mem[_mTypeSID];
    mS_B_CPU[_mInitSID]=s_B_CPU[_mTypeSID];
    mS_B_MEM[_mInitSID]=s_B_Mem[_mTypeSID];
}

//服务器恢复初始默认状态,复原的服务器ID
void revertServer(int _mRevertSID){
    int mSType=mS_TypeID[_mRevertSID];
    mS_VMNum[_mRevertSID]=0;//此服务器虚拟机的个数
    mS_A_CPU[_mRevertSID]=s_A_CPU[mSType];
    mS_A_MEM[_mRevertSID]=s_A_Mem[mSType];
    mS_B_CPU[_mRevertSID]=s_B_CPU[mSType];
    mS_B_MEM[_mRevertSID]=s_B_Mem[mSType];
}

//释放删除虚拟机带来的计算机资源释放,虚拟机ID
bool releaseRes(int mReleaseVMID){
    int temVMNum=-1;//存储虚拟机信息的下标
    for(int i=0;i<100000;i++){
        if (isHaveVM[i]==true && mVM_ID[i]==mReleaseVMID){   
            temVMNum=i;
            break;
        }  
    }
    if (temVMNum==-1){outInfo.push_back("释放硬件资源时失败，没有找到对应的虚拟机ID:"+std::to_string(mReleaseVMID)+"\n"); return false;}//如果没有得到找到存在虚拟机直接退出
    //判断虚拟机标志位,确实存在此虚拟机
    
    if (mS_VMNum[mVM_inServerID[temVMNum]]>0){//服务器的虚拟机个数>0
        int temSID=mVM_inServerID[temVMNum];//服务器ID
        int temVMID=mVM_TypeID[temVMNum];//虚拟机的类型ID
        switch (mVM_NodePlace[temVMNum]){
        case 0://双节点
            if(VM_isDouleNode[temVMID]!=1){//本身虚拟机就要求双节点
                outInfo.push_back("删除虚拟机时，VM本身节点要求和实际部署不符合,节点值: "+ std::to_string(temVMID));
                return false;
            }
            mS_VMNum[temSID]-=1;//此服务器虚拟机的个数
            mS_A_CPU[temSID]+=VM_CPU[temVMID]/2;//实时的剩余硬件资源
            mS_A_MEM[temSID]+=VM_MEM[temVMID]/2;//实时的剩余硬件资源
            mS_B_CPU[temSID]+=VM_CPU[temVMID]/2;//实时的剩余硬件资源
            mS_B_MEM[temSID]+=VM_MEM[temVMID]/2;//实时的剩余硬件资源
            return true;
            break;
        case 1://A节点
            if(VM_isDouleNode[temVMID]!=0){//本身虚拟机就要求单节点
                outInfo.push_back("删除虚拟机时，VM本身节点要求和实际部署不符合,节点值: "+ std::to_string(temVMID));
                return false;
            }
            mS_VMNum[temSID]-=1;//此服务器虚拟机的个数
            mS_A_CPU[temSID]+=VM_CPU[temVMID];//实时的剩余硬件资源
            mS_A_MEM[temSID]+=VM_MEM[temVMID];//实时的剩余硬件资源
            return true;
            break;
        case 2://B节点
            if(VM_isDouleNode[temVMID]!=0){//本身虚拟机就要求单节点
                outInfo.push_back("删除虚拟机时，VM本身节点要求和实际部署不符合,节点值: "+ std::to_string(temVMID));
                return false;
            }
            mS_VMNum[temSID]-=1;//此服务器虚拟机的个数
            mS_B_CPU[temSID]+=VM_CPU[temVMID];//实时的剩余硬件资源
            mS_B_MEM[temSID]+=VM_MEM[temVMID];//实时的剩余硬件资源
            return true;
            break;
        default:
            outInfo.push_back("删除虚拟机时，遇到节点判断错误,节点值: "+ std::to_string(mVM_NodePlace[temVMNum]));
            return false;
            break;
        }
    }else{
        outInfo.push_back("释放资源，发现服务上不存在需要释放的虚拟机");return false;
    }
    return false;
}

//购买一种服务器,服务器类型ID，数量
void buyServer(int buySTypeID, int buySNum){
    for(int i=0;i<buySNum;i++){
        initServer(allBuySNum,buySTypeID);//用指定类型服务器类型，初始化指定服务器
        allBuySNum+=1;
    }
 }

 //删除一个虚拟机,直接把标志位，待删除虚拟机的ID
bool delVM(int mDelVMID){
    if (releaseRes(mDelVMID)){//释放资源成功
       for(int i=0;i<100000;i++){
            if (isHaveVM[i]==true && mVM_ID[i]==mDelVMID){   
                isHaveVM[i]=false;//把保存实际虚拟机信息的标志位置false
                mS_isNoSpaceInstall[mVM_inServerID[i]]=false;//服务器如果原先不能再装VM，这次以后可再次安装VM
                return true;
            }  
        }
        // temVmOpInfo.push_back("删除虚拟机时，资源释放完成，但是虚拟机的标志位置false失败\n");
        outInfo.push_back("删除虚拟机时，资源释放完成，但是虚拟机的标志位置false失败\n");
        return false;
    }
    outInfo.push_back("删除虚拟机失败\n");
    // temVmOpInfo.push_back("删除虚拟机失败\n");
    return false;
 }

//按照指定虚拟机的类型,节点布置,拿走相应服务器的计算机资源,实际服务器ID,预安装虚拟机类型ID,节点布置
bool setUpVM(int &_mSID, int &_mVM_TypeID, int _mNode){
    if (_mSID<allBuySNum && _mVM_TypeID<VMTypeNum){//此服务器ID存在且此虚拟机的类型存在
        int temACPU=0,temAMEM=0,temBCPU=0,temBMEM=0;
        int temCPU1=0,temMEM1=0;
        int temCPU2=0,temMEM2=0;
        switch (_mNode)
        {
        
        case 0://双节点
            
            temACPU = mS_A_CPU[_mSID]-VM_CPU[_mVM_TypeID]/2;
            temAMEM = mS_A_MEM[_mSID]-VM_MEM[_mVM_TypeID]/2;
            temBCPU = mS_B_CPU[_mSID]-VM_CPU[_mVM_TypeID]/2;
            temBMEM = mS_B_MEM[_mSID]-VM_MEM[_mVM_TypeID]/2;
            if ( temACPU>1 && temAMEM>1 && temBCPU>1 && temBMEM>1)
            {
                mS_VMNum[_mSID]+=1;//此服务器虚拟机的个数，若不为0则Run
                mS_A_CPU[_mSID]=temACPU;//实时的剩余硬件资源
                mS_A_MEM[_mSID]=temAMEM;//实时的剩余硬件资源
                mS_B_CPU[_mSID]=temBCPU;//实时的剩余硬件资源
                mS_B_MEM[_mSID]=temBMEM;//实时的剩余硬件资源 
                outInfo.push_back("("+to_string(_mSID)+")\n");
                // temVmOpInfo.push_back("("+to_string(_mSID)+")\n");
                return true;
                break;
            }
            return false;
            break;
        case 1://A节点
            
            temCPU1 = mS_A_CPU[_mSID] - VM_CPU[_mVM_TypeID];
            temMEM1 = mS_A_MEM[_mSID] - VM_MEM[_mVM_TypeID];
            if (temCPU1>1 && temMEM1>1){
                mS_VMNum[_mSID] += 1;//此服务器虚拟机的个数
                mS_A_CPU[_mSID] = temCPU1;//实时的剩余硬件资源
                mS_A_MEM[_mSID] = temMEM1;//实时的剩余硬件资源
                outInfo.push_back("("+to_string(_mSID)+", A)\n");
                // temVmOpInfo.push_back("("+to_string(_mSID)+", A)\n");
                return true;
                break;
            }
            return false;
            break;
        case 2://B节点
            
            temCPU2 = mS_B_CPU[_mSID] - VM_CPU[_mVM_TypeID];
            temMEM2 = mS_B_MEM[_mSID] - VM_MEM[_mVM_TypeID];
            if (temCPU2>1 && temMEM2>1){
                mS_VMNum[_mSID] += 1;//此服务器虚拟机的个数
                mS_B_CPU[_mSID] = temCPU2;//实时的剩余硬件资源
                mS_B_MEM[_mSID] = temMEM2;//实时的剩余硬件资源
                outInfo.push_back("("+to_string(_mSID)+", B)\n");
                // temVmOpInfo.push_back("("+to_string(_mSID)+", B)\n");
                return true;
                break;
            }
            return false;
            break;
        default:
            // temVmOpInfo.push_back("请求资源时，遇到节点判断错误,节点值: " + std::to_string(_mNode));
            outInfo.push_back("请求资源时，遇到节点判断错误,节点值: " + std::to_string(_mNode));
            return false;
        }
    }
    // temVmOpInfo.push_back("请求资源时，没有对应的服务器ID或虚拟机类型ID \n");
    outInfo.push_back("请求资源时，没有对应的服务器ID或虚拟机类型ID \n");
    return false;
}

//采用算法随机一个数
int algoRandNum(int beign, int end){
    srand(allBuySNum);
    int temrand=beign + rand() % end;
    return temrand;
}

//判断每日额服务器购买的额硬件成本加上能耗成本是否合规，硬件成本不超过100000，能耗成本不超过5000
bool dayHardPowerCost(){

}

// 迁移虚拟机策略
void migrate(){
    outInfo.push_back("(migration, 0)\n");
}

//创建虚拟机时资源不足再次购买
bool againBuyServer(){

    if(outInfo.back().substr(0,10) == "(migration"){
        outInfo.pop_back();//除去原先的迁移，因为没有迁移所以，只用除去最后一次输出，以后需要迁移时，需要更改此项
    }
    vector<string> eachTypeNum;//存储以前购买的具体服务器的信息
    
    if(outInfo.back() == "(purchase, 0)"){//前面没有购买服务器
        outInfo.pop_back();//删除(purchase, 0)
        int temSTypeID = algoRandNum(0,s_TypeNum-1);
        buyServer(temSTypeID,2);
        outInfo.push_back("(purchase, "+std::to_string(1)+")\n");
        outInfo.push_back("(" + s_Name[temSTypeID] + ", " + std::to_string(2) + ")\n");
    }else {//前面已经购买了一种服务器，服务器信息类似(hostE8YFB, 200)
        // index=outInfo.size();
        // cout<<"1:"<<outInfo.at(index-1)<<endl;
        // cout<<"2:"<<outInfo.at(index-2)<<endl;
        // cout<<"3:"<<outInfo.at(index-3)<<endl;
        eachTypeNum.push_back(outInfo.back());
        outInfo.pop_back();//删除服务器信息类似(hostE8YFB, 200)
        // cout<<"2:"<<outInfo.back()<<endl;
        outInfo.pop_back();//再删除(purchase, 1)
        // cout<<"3:"<<outInfo.back()<<endl;
        string lastBuySTypeStr=eachTypeNum.back().substr(1,9);//得到上一次购买的服务器型号如hostE8YFB
        int lastSTypeID=getSTypeID(lastBuySTypeStr);
        int againBuySTypeID = lastSTypeID/2;//得到新购买服务器的类型ID，除2是为了获得新类型
        if(lastSTypeID==againBuySTypeID){//防止两次购买一样的服务器
            againBuySTypeID++;
        }
        buyServer(againBuySTypeID,2);//按照新服务器类型ID，购买2台
        outInfo.push_back("(purchase, "+std::to_string(2)+")\n");
        outInfo.push_back(eachTypeNum.back());//以前的购买信息
        outInfo.push_back("(" + s_Name[againBuySTypeID] + ", " + std::to_string(2) + ")\n");
    }
    
    return true;
}

//创建虚拟机时资源不足再次购买
void repeatBuyServer(){
    int temSTypeID = algoRandNum(0,s_TypeNum-1);
    buyServer(temSTypeID,3);
    temBuyTypeNum[temSTypeID]=temBuyTypeNum[temSTypeID]+3;
}

//统计一天中购买服务器的类型和数量
void countSTypeNum(unordered_map<int,int> &_temBuyTypeNum){
    int temNum = _temBuyTypeNum.size();
    temBuyS.push_back("(purchase, "+std::to_string(temNum)+")\n");
    for (auto it : _temBuyTypeNum) {
        if (it.second!=0){//此服务器类型购买数量为0，则不输出
            temBuyS.push_back("(" + s_Name[it.first] + ", " + std::to_string(it.second) + ")\n");//(NV603, 2)
        }
    }
}

//创建虚拟机
bool createVM(string &createVMName, int &mVMID){
    int temVMTypeID=-1;//虚拟机类型ID
    for(int i=0;i<MAX_VMType;i++){
        if(VM_Name[i]==createVMName){
            temVMTypeID=i;
            break;
        }
    }
    if (temVMTypeID==-1){temVmOpInfo.push_back("创建虚拟机时，没有找到对应虚拟机类型，创建失败"); return false;}//
    
    //指定虚拟机安装的服务器ID和安装节点位置
    switch (VM_isDouleNode[temVMTypeID])//按照虚拟机要求的单双节点来安装vm
    {
    case 1://双节点安装请求
        for(int i=0;i<allBuySNum;i++){
            if (mS_isNoSpaceInstall[i]== false){
                if(setUpVM(i, temVMTypeID, 0)){//当前服务器安装此虚拟机成功
                    for(int vmWhere=0;vmWhere<100000; vmWhere++){
                        if (isHaveVM[vmWhere]==false){
                            isHaveVM[vmWhere]=true;//保存虚拟机信息的标志位
                            mVM_ID[vmWhere]=mVMID;//保存虚拟机ID
                            mVM_TypeID[vmWhere]=temVMTypeID;//保存虚拟机类型ID
                            mVM_inServerID[vmWhere]=i;//虚拟机所在服务器ID
                            mVM_NodePlace[vmWhere]=0;//AB节点
                            return true;
                        }
                    }
                }else
                {
                    mS_isNoSpaceInstall[i]=true;
                }
            }
        }
        // againBuyServer();
        // createVM(createVMName, mVMID);
        // temVmOpInfo.push_back("按照双节点布置虚拟机失败，请检查是否有足够的服务器资源\n");
        return false;
        break;
    case 0://单节点安装请求
        for(int i=0;i<allBuySNum;i++){
            if (mS_isNoSpaceInstall[i]== false){
                if(setUpVM(i, temVMTypeID, 1)){//服务器A节点尝试安装成功
                    for(int vmWhere=0;vmWhere<100000; vmWhere++){
                        if (isHaveVM[vmWhere]==false){
                            isHaveVM[vmWhere]=true;//保存虚拟机信息的标志位
                            mVM_ID[vmWhere]=mVMID;//保存虚拟机ID
                            mVM_TypeID[vmWhere]=temVMTypeID;//保存虚拟机类型ID
                            mVM_inServerID[vmWhere]=i;//虚拟机所在服务器ID
                            mVM_NodePlace[vmWhere]=1;//A节点
                            return true;
                        }
                    }
                }else if(setUpVM(i, temVMTypeID, 2)){//服务器B节点尝试安装成功
                    for(int vmWhere=0;vmWhere<100000; vmWhere++){
                        if (isHaveVM[vmWhere]==false){
                            isHaveVM[vmWhere]=true;//保存虚拟机信息的标志位
                            mVM_ID[vmWhere]=mVMID;//保存虚拟机ID
                            mVM_TypeID[vmWhere]=temVMTypeID;//保存虚拟机类型ID
                            mVM_inServerID[vmWhere]=i;//虚拟机所在服务器ID
                            mVM_NodePlace[vmWhere]=2;//B节点
                            return true;
                        }
                    }
                }else{
                    mS_isNoSpaceInstall[i]=true;
                }
            }
        }
        
        // againBuyServer();
        // createVM(createVMName, mVMID);
        // temVmOpInfo.push_back("按照单节点布置虚拟机失败，请检查是否有足够的服务器资源\n");
        return false;
        break;
    default:
        temVmOpInfo.push_back("创建虚拟机时，提供的节点(单/双)，有误\n");
        return false;
        break;
    }
    
}
// 扩容服务器策略
void expansion(const int &dayRequestNumber,vector<vector<string>> &request){
    //是否具备扩容条件
    if (allBuySNum!=0 && allBuySNum<100000)//已有服务器，后续的扩容处理
    {
        int temInstallNum=0;
        for(int i=0;i<allBuySNum;i++){
            if (mS_isNoSpaceInstall[i]==false){
                temInstallNum++;
            }
        }
        int newServerNum=0;//确定新购买的服务器数量
        for (auto it : request) {
            // std::cout<<"扩容："<<it[0][0]<<endl;           
            if(it[0][0]=='a'){
                ++newServerNum;
            }else if (it[0][0]=='d'){
                --newServerNum;
            }
        }
        
        newServerNum = (newServerNum-temInstallNum)/2;
        if (newServerNum<=0){
            outInfo.push_back("(purchase, "+std::to_string(0)+")\n");
            return;
        } else{
            int temSTypeID = algoRandNum(0,s_TypeNum-1);
            // int temSTypeID = 28;
            buyServer(temSTypeID,newServerNum);
            outInfo.push_back("(purchase, "+std::to_string(1)+")\n");
            outInfo.push_back("(" + s_Name[temSTypeID] + ", " + std::to_string(newServerNum) + ")\n");
        }
    }else if (allBuySNum==0)//第一次购买服务器
    {   
        // int temSTypeID1 = algoRandNum(0,s_TypeNum-1);
        // int temSTypeBuyNum1 = algoRandNum(1,100);
        int temSTypeID1 = s_TypeNum/5;
        buyServer(temSTypeID1,200);

        // int temSTypeID2 = algoRandNum(0,s_TypeNum-1);
        // int temSTypeBuyNum2 = algoRandNum(1,100);
        int temSTypeID2 = s_TypeNum/4;
        buyServer(temSTypeID2,220);

        // int temSTypeID3 = algoRandNum(0,s_TypeNum-1);
        // int temSTypeBuyNum3 = algoRandNum(1,100);
        int temSTypeID3 = s_TypeNum/3;
        buyServer(temSTypeID3,220);

        // int temSTypeID4 = algoRandNum(0,s_TypeNum-1);
        // int temSTypeBuyNum4 = algoRandNum(1,100);
        int temSTypeID4 = s_TypeNum/2;
        buyServer(temSTypeID4,220);

        int temSTypeID5 = s_TypeNum/6;
        buyServer(temSTypeID5,220);

        int temSTypeID6 = s_TypeNum/8;
        buyServer(temSTypeID6,220);

        int temSTypeID7 = s_TypeNum/7;
        buyServer(temSTypeID7,220);

        // int allBuySNum=temSTypeBuyNum1+temSTypeBuyNum2+temSTypeBuyNum3+temSTypeBuyNum4;
        outInfo.push_back("(purchase, "+std::to_string(7)+")\n");
        outInfo.push_back("(" + s_Name[temSTypeID1] + ", " + std::to_string(220) + ")\n");//(NV603, 2)
        outInfo.push_back("(" + s_Name[temSTypeID2] + ", " + std::to_string(220) + ")\n");
        outInfo.push_back("(" + s_Name[temSTypeID3] + ", " + std::to_string(220) + ")\n");
        outInfo.push_back("(" + s_Name[temSTypeID4] + ", " + std::to_string(220) + ")\n");
        outInfo.push_back("(" + s_Name[temSTypeID5] + ", " + std::to_string(220) + ")\n");
        outInfo.push_back("(" + s_Name[temSTypeID6] + ", " + std::to_string(220) + ")\n");
        outInfo.push_back("(" + s_Name[temSTypeID7] + ", " + std::to_string(220) + ")\n");
    }else{
        outInfo.push_back("不能扩容了");
    }
    
}


void beginAnalysis(const int &dayRequestNum,vector<vector<string>> &requests){
    expansion(dayRequestNum,requests);//采用扩容策略，内部判断是否购买新增服务器
    migrate();//迁移虚拟机的策略
    //开始处理创建和删除操作
    // temBuyS.clear();
    // temBuyTypeNum.clear();
    // temVmOpInfo.clear();//开始之前，清空之前可能存在的vm操作结果
    
    for(auto req:requests){
        // 创建虚拟机
        if(req.size() == 3){
            int temVmID=atoi(req[2].c_str());
            createVM(req[1],temVmID);
            // while (!createVM(req[1],temVmID)){
            //     repeatBuyServer();//创建不成功时，再次购买服务器
            // }
        }
        //删除虚拟机
        else if (req.size() == 2){
            int temVmID = atoi(req[1].c_str());
            delVM(temVmID);
        }else{
            outInfo.push_back("请求指令解析时，发现未知指令，可能是保存的指令信息有误，正常为:add,del等");
        }
    }
    //把当天的处理结果保存到总输出中

    // countSTypeNum(temBuyTypeNum);
    // outInfo.insert(outInfo.end(),temBuyS.begin(),temBuyS.end());//把服务器购买信息汇总到总输出
    // migrate();
    // outInfo.insert(outInfo.end(),temVmOpInfo.begin(),temVmOpInfo.end());//把操作信息汇总到中输出
}

int main() {
    // std::freopen("C:\\Users\\Administrator\\Desktop\\HELLOWORLD\\training-data\\training-2.txt","rb",stdin);
    
    setServerAllType();
    setVmAllType();//得到总的虚拟机类型参数
    
    // vector<string> qingqiu;
    //请求总天数
    scanf("%d",&allRequestDayNum);
 // 开始处理请求
    int dayRequestNumber = 0;//每天的请求数
    string op,reqVmType,reqId;//操作，虚拟机的型号，虚拟机ID
    for(int day=0;day<allRequestDayNum;day++){
        scanf("%d",&dayRequestNumber);//得到每一天的请求数
        
        dayRequestInfos.resize(0);
        for(int i =0;i<dayRequestNumber;i++){
            cin>>op;
            if(op[1] == 'a'){
                cin>>reqVmType>>reqId;
                generateRequest(op,reqVmType,reqId);//保存读取到创建请求信息
            }else if(op[1] == 'd'){ 
                cin>>reqId;
                generateRequest(op,reqId);//保存读取到删除请求信息
            }
        }
        // for(auto info:dayRequestInfos){
        //     qingqiu.insert(qingqiu.end(),info.begin(),info.end());
        // }
        beginAnalysis(dayRequestNumber,dayRequestInfos);     //进行每一天的分配

    }
    fclose(stdin);

    for (auto &s:outInfo) std::cout<<s;

    // char fname[] = "C:\\Users\\Administrator\\Desktop\\HELLOWORLD\\training-data\\out-2.txt";
    // ofstream fout(fname);  // 建一个输出流对象和文件关联 // 默认是ios::out, ios::app表示在打开文件末尾添加内容
    // if(!fout){
    //     cout << "open error" << endl;
    //     return 0;
    // }
    // for (auto &s:outInfo) fout<<s;
    // fout.close();

    // char fname1[] = "C:\\Users\\Administrator\\Desktop\\HELLOWORLD\\training-data\\qingqiu.txt";
    // ofstream fout1(fname1);  // 建一个输出流对象和文件关联 // 默认是ios::out, ios::app表示在打开文件末尾添加内容
    // if(!fout1){
    //     cout << "open error" << endl;
    //     return 0;
    // }
    // for (auto &s1:qingqiu) fout1<<s1<<endl;
    // fout1.close();

    return 0;
}