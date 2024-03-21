#include "Room.h"
#include <algorithm>
#include <cassert>
#include "Socket/MessageQueue.h"
#include "Socket/SocketConfig.h"
#include <cmath>

int Room::GetPlayerIndex(std::shared_ptr<Player> player)
{
    auto ite = std::find(mPlayers.begin(), mPlayers.end(), player);
    assert(ite != mPlayers.end());
    return ite - mPlayers.begin();
}

void Room::SendRoomDataNtf(std::vector<std::shared_ptr<Player>>& players)
{

    auto ntf = std::make_shared<XTankMsg::RoomNtf>();
    ntf->set_isingame(roomState == ROOM_STATE::GAMING);


    for (auto& player : mPlayers) {

        if (player) {
            auto playerState = ntf->add_playerstates();
            playerState->set_ready(player->state != PlAYER_STATE::IDLE);

            XTankMsg::PlayerId ppid;
            ppid.set_ip(player->id.ip);
            ppid.set_pid(player->id.pid);

            playerState->mutable_playerid()->CopyFrom(ppid);
        }
    }

    for (auto& player : players) {
        MsgSendQueue::Instance().PushMsg({ player->id,MessageData{XTankMsg::ROOM_NTF,ntf} });

    }


}

void Room::SendGameReadyReq(const std::vector<std::shared_ptr<Player>>& players)
{
    for (auto& player : players) {
        player->latency.StartCount();
        MsgSendQueue::Instance().SendGameReadyReq(player->id);
    }
}

void Room::SendGameInitNtf(const std::vector<std::shared_ptr<Player>>& players)
{

    auto ntf = std::make_shared<XTankMsg::GameInitNtf>();

    ntf->set_randseed(0);

    //注意用的是开局时的玩家列表 正在加入的玩家不算入其中
    if (roomState == ROOM_STATE::WAITING_START) {
        initPlayers = GetPlayersNotInState(PlAYER_STATE::CUT_IN);
    }

    for (auto& player : initPlayers) {
       
        auto ppid = ntf->add_playerids();
        ppid->set_ip(player->id.ip);
        ppid->set_pid(player->id.pid);
    }

    for (auto& player : players) {

        MsgSendQueue::Instance().PushMsg({ player->id,MessageData{XTankMsg::GAME_INIT_NTF,ntf} });
    }

}


void Room::SendGameStartNtf(std::vector<std::shared_ptr<Player>>& players)
{

    auto ntf = std::make_shared<XTankMsg::GameStartNtf>();

    ntf->set_latency(0);

    for (auto& player : players) {
        MsgSendQueue::Instance().PushMsg({ player->id,MessageData{XTankMsg::GAME_START_NTF,ntf} });
    }
}

void Room::SendGameForwardNtf(std::vector<std::shared_ptr<Player>>& players)
{

    auto ntf = std::make_shared<XTankMsg::GameForwardNtf>();

    int frameId = gamingTick.GetFrameId();
    ntf->set_frameid(frameId);

    for (BUTTON::Type btn : playersCmdMap[frameId]) {

        ntf->add_playercmds(btn);
    }

    for (auto& player : players) {
        MsgSendQueue::Instance().PushMsg({ player->id,MessageData{XTankMsg::GAME_FORWARD_NTF,ntf} });
    }

}

void Room::SendPlayerCutInNtf(std::vector<std::shared_ptr<Player>>& players)
{
    auto ntfCutin = std::make_shared<XTankMsg::PlayerCutInNtf>();

    //添加房间内玩家信息
    for (auto& player : mPlayers) {

        if (player) {
            PlayerId& playerId = player->id;

            auto playerIdMsg = ntfCutin->add_playerids();
            playerIdMsg->set_ip(playerId.ip);
            playerIdMsg->set_pid(playerId.pid);
        }
    }

    int curFrameId = gamingTick.GetFrameId();

    //添加之前所有帧
    for (const auto& [frameId, cmds] : playersCmdMap) {

        //不发送还未转发的指令
        if (frameId > curFrameId) {
            continue;
        }

        auto cmdMsg = ntfCutin->add_cmds();
        cmdMsg->set_frameid(frameId);

        for (BUTTON::Type cmd : cmds) {
            cmdMsg->add_playercmds(cmd);
        }
    }

    //需发送之前的所有命令
    for (auto& player : players) {

        MsgSendQueue::Instance().PushMsg({ player->id,MessageData { XTankMsg::PLAYER_CUT_IN_NTF,ntfCutin } });
    }
}


void Room::GameStartInit()
{

    //设置更新帧率 并且延时执行
    gamingTick.SetTickTime(LOCKSTEP_TICK);
    //延迟
    gamingTick.SetDelay(SIMULATE_PING);
}

void Room::JudgePlayerCutIn()
{

    auto cutInPlayers = GetPlayersInState(PlAYER_STATE::CUT_IN);
    
    if (!cutInPlayers.empty()) {

        roomState = ROOM_STATE::WAITING_CUT_IN;

        //发送游戏准备请求
        SendGameReadyReq(cutInPlayers);
        //发送游戏初始化通知
        SendGameInitNtf(cutInPlayers);
    }

}

void Room::JudgePlayerChaseUp()
{

    auto chaseUpPlayers = GetPlayersInState(PlAYER_STATE::CHASE_UP);

    for (auto& player : chaseUpPlayers) {

        player->state = PlAYER_STATE::GAMING;
        int index = GetPlayerIndex(player);
        additionPlayersCmd[index] = BUTTON::CUT_IN;
    }

}

void Room::AddAdditionCmdToPlayersCmd()
{
    int frameId = gamingTick.GetFrameId();

    auto& curCmds = playersCmdMap[frameId];

    for (int i = 0; i < curCmds.size(); ++i) {
        if (additionPlayersCmd[i] != BUTTON::NONE) {
            curCmds[i] = additionPlayersCmd[i];
        }
    }

    additionPlayersCmd.fill(BUTTON::NONE);
}

std::vector<std::shared_ptr<Player>> Room::GetPlayersInState(PlAYER_STATE::Type state)
{
    std::vector<std::shared_ptr<Player>> players;

    for (auto& player : mPlayers) {
        if (player && player->state == state) {
            players.push_back(player);
        }
    }

    return players;
}

std::vector<std::shared_ptr<Player>> Room::GetPlayersNotInState(PlAYER_STATE::Type state)
{
    std::vector<std::shared_ptr<Player>> players;

    for (auto& player : mPlayers) {
        if (player && player->state != state) {
            players.push_back(player);
        }
    }

    return players;
}

void Room::Init()
{
    outGameTick.SetTickTime(OUT_GAME_MSG_TICK);
    gamingTick.SetTickTime(LOCKSTEP_TICK);
}

void Room::RemovePlayer(std::shared_ptr<Player> player)
{
    int index = GetPlayerIndex(player);

    if (mPlayers[index]) {
        
        if (mPlayers[index]->state == PlAYER_STATE::GAMING) {
            //需加上退出指令
            additionPlayersCmd[index] = BUTTON::EXIT;
        }

        mPlayers[index].reset();
    }
}

void Room::AddPlayer(std::shared_ptr<Player> player)
{
    assert(GetPlayersNum() < 4);
    for (auto& inPlayer : mPlayers) {
        if (inPlayer == nullptr) {
            inPlayer = player;
            break;
        }
    }
}

void Room::AddPlayerGameInput(std::shared_ptr<Player> player, int frameId, BUTTON::Type btnType)
{
    int index = GetPlayerIndex(player);
    int curFrameId = gamingTick.GetFrameId();
    //如果用户帧id小于服务器帧id 则说明其延迟较高已经错过转发
    //直接将此命令放到下一帧转发即可
    frameId = std::max(curFrameId + 1, frameId);
    playersCmdMap[frameId][index] = btnType;
}

int Room::GetPlayersNum()
{
    int size = std::count_if(mPlayers.begin(), mPlayers.end(),
        [](const std::shared_ptr<Player>& player) {return player != nullptr; });
    return size;
}


bool Room::IsInGame()
{

    return !(roomState == ROOM_STATE::OUT_GAME);
}

bool Room::JudgeIsAllReady()
{

    auto readyPlayers = GetPlayersInState(PlAYER_STATE::READY);
    int totalPlayerNum = GetPlayersNum();
    //是否都已准备
    if (!readyPlayers.empty() && readyPlayers.size() == totalPlayerNum) {
       
        roomState = ROOM_STATE::WAITING_START;
        //发送游戏准备请求
        SendGameReadyReq(readyPlayers);
        //发送游戏初始化通知
        SendGameInitNtf(readyPlayers);
    }

    return true;
}

bool Room::IsStart()
{
    return roomState == ROOM_STATE::GAMING || JudgeCanStart();
}

bool Room::JudgeCanStart()
{

    auto readyStartPlayers = GetPlayersInState(PlAYER_STATE::READY_START);
    int totalPlayerNum = GetPlayersNum();

    if (!readyStartPlayers.empty() && readyStartPlayers.size() == totalPlayerNum) {
        for (auto& player : readyStartPlayers) {
            player->state = PlAYER_STATE::GAMING;
        }

        roomState = ROOM_STATE::GAMING;

        //发送游戏开始通知
        SendGameStartNtf(readyStartPlayers);

        GameStartInit();
    }

    return true;
}

bool Room::JudgeCanStartCutIn()
{
    std::vector<std::shared_ptr<Player>> cutInPlayers;

    //是否都已等待开始
    for (auto& player : mPlayers) {
        if (player) {
            if (player->state == PlAYER_STATE::READY_START) {
                cutInPlayers.push_back(player);
            }
            else if (player->state != PlAYER_STATE::GAMING) {
                return false;
            }
        }
    }

    roomState = ROOM_STATE::GAMING;

    //发送玩家加入通知
    SendPlayerCutInNtf(cutInPlayers);

    for (auto& player : cutInPlayers) {
        player->state = PlAYER_STATE::GAMING;
    }


    return true;
}

void Room::Tick()
{

    switch (roomState)
    {
    case ROOM_STATE::OUT_GAME:

        OutGameTick();
        JudgeIsAllReady();
        break;

    case ROOM_STATE::WAITING_START:

        JudgeCanStart();
        break;

    case ROOM_STATE::GAMING:

        JudgePlayerCutIn();
        JudgePlayerChaseUp();
        GamingTick();
        OutGameTick();
        break;

    case ROOM_STATE::WAITING_CUT_IN:

        JudgeCanStartCutIn();
        GamingTick();
        break;

    default:
        break;
    }

}

void Room::GamingTick()
{

    gamingTick.Tick();
    if (gamingTick.IsNeedTick()) {
        
        AddAdditionCmdToPlayersCmd();

        auto gamingPlayers = GetPlayersInState(PlAYER_STATE::GAMING);
        
        SendGameForwardNtf(gamingPlayers);
    }
}

void Room::OutGameTick()
{

    outGameTick.Tick();
    if (outGameTick.IsNeedTick()) {
        
        auto outGamePlayers = GetPlayersNotInState(PlAYER_STATE::GAMING);

        SendRoomDataNtf(outGamePlayers);

    }
}
