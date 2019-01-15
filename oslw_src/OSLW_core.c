/*(Ver.=0.9~)(Beg.=0.9)
 * OSLW_core.c
 *
 *  Created on: 2017-7-14
 *      Author: slw
 */

#include "OSLW_include.h"


#if OSLW_OS_ENABLE


//------------------------------------------
//<������>OSlwTaskDispatch</������>
//<����˵��>��������� ����ϵͳ����</����˵��>
//<����˵��>pOS:this</����˵��>
//<���˵��>void</���˵��>
//------------------------------------------
void OSlwTaskDispatch(OSlwCoreSTU *_pOS)
{
    static OSlwCoreSTU *pOS;
    OSLW_assert(!_pOS);

    pOS = _pOS;

    if(!(pOS->DispatchEable))//�жϵ������Ƿ�����
    {
        while(pOS->pCurrentTask->TaskStatus != OSlwTaskStatus_Ready);//ֱ�ӵȴ��������׼��״̬

        return ;
    }
		
    if(pOS->InitialFlagGroup.all)//�жϳ�ʼ����־���Ƿ�Ϊ0
    {   //�����Ϊ0 ˵��������δ��ʼ��������
        for(pOS->n1_Dispatch=0; 
			pOS->n1_Dispatch<pOS->TaskRunningNum
#if OSLW_SIMPLE_MODE
			-1
#endif			
			; pOS->n1_Dispatch++) //ѭ������δ��ʼ��������  //---------���Բ����㷨�Ż�--------//
        {
            pOS->Temp=pOS->TaskRunningList[pOS->n1_Dispatch];
            if((pOS->InitialFlagGroup.all & ((lw_u64)1<<(pOS->Temp))) && //�����ʼ����־���ĳһλΪ1
                    (pOS->pTaskList[pOS->Temp]))//����������Ѿ�ע����
            {
                pOS->pCurrentTask=pOS->pTaskList[pOS->Temp];
#if !OSLW_SIMPLE_MODE
                if(!_OSLW_SAVE(pOS->DispatchBreakPoint))//�ȴ�����������
                {
                    if(pOS->pCurrentTask->TaskFun)//��ת֮ǰ���жϺ���ָ���Ƿ�Ϊ0
                    {
                        pOS->pCurrentTask->TaskFun(pOS->pCurrentTask);
                    }//end if

                }//end if
#else
								if(pOS->pCurrentTask->TaskFun)//��ת֮ǰ���жϺ���ָ���Ƿ�Ϊ0
								{
										pOS->pCurrentTask->TaskFun(pOS->pCurrentTask);
								}//end if								
#endif
								
                pOS->InitialFlagGroup.all&=(~((lw_u64)1<<(pOS->Temp)));//�����ʼ����־λ
                pOS->ReadyFlagGroup.all|=((lw_u64)1<<(pOS->Temp));
								
#if	!OSLW_SIMPLE_MODE															
                pOS->pLastTask=pOS->pCurrentTask;
                pOS->pCurrentTask->TaskStatus=OSlwTaskStatus_Ready;
#endif
								
            }//end if
        }//end for
    }
#if (OSLW_SIMPLE_MODE)
	do{	
#endif	//(OSLW_SIMPLE_MODE)		
    pOS->_flag_BK = pOS->ReadyFlagGroup.all & (~((lw_u64)1 << (OSLW_TASK_NUM_MAX - 1)));
    if(pOS->_flag_BK)//����������ȼ�--�������� �鿴�����Ƿ�׼�����
    {
        pOS->_ReadyTaskIndex = 0;
        if (pOS->TaskRunningNum > __OSLW_LOG2_TASK_NUM  )//������������Ƚ϶� ˵�����ַ�����ʱ�临�ӶȽϵ�
        {

            //���ַ�
            for (pOS->n2_Dispatch = 0; pOS->n2_Dispatch < __OSLW_LOG2_TASK_NUM - 2; pOS->n2_Dispatch++)//���ַ��۰뵽4 ֱ��switch case
            {
                /*(Ver.=0.9~)(Beg.=0.9)
                	TASK_NUM == 8 Ϊ��
                	ReadyFlagGroup=0b10010010
                	ReadyFlagGroup & 0b00001111 �൱��ȡ����4λ
                	�жϵ���λ�Ƿ�Ϊ0
                	�������λ ��Ϊ0 ˵�� ����λ�б����������
                	��֮ �������λ�ڸ�4

                */
                if (pOS->_flag_BK & pOS->_mask_list[pOS->n2_Dispatch])
                {
                    //�����Ϊ0
                    pOS->_flag_BK &= pOS->_mask_list[pOS->n2_Dispatch];//���θ�λ
                }
                else
                {
                    pOS->_flag_BK = (pOS->_flag_BK >> pOS->_move_reg[pOS->n2_Dispatch]) & pOS->_mask_list[pOS->n2_Dispatch];//�õ���λ
                    pOS->_ReadyTaskIndex += pOS->_move_reg[pOS->n2_Dispatch];
                }

            }//end for

            if (pOS->_flag_BK < 16)
            {
                switch (pOS->_flag_BK)
                {
                case 8:
                    pOS->_ReadyTaskIndex += 3;
                    break;
                case 4:
                    pOS->_ReadyTaskIndex += 2;
                    break;
                case 12:
                    pOS->_ReadyTaskIndex += 2;
                    break;
                case 2:
                    pOS->_ReadyTaskIndex += 1;
                    break;
                case 6:
                    pOS->_ReadyTaskIndex += 1;
                    break;
                case 10:
                    pOS->_ReadyTaskIndex += 1;
                    break;
                case 14:
                    pOS->_ReadyTaskIndex += 1;
                    break;

                default://ĩβ��1
                    break;
                }
            }
            else
            {
                OSLW_assert(1);//��Ӧ��>=16
            }
        }// end if (pOS->TaskRunningNum >= __OSLW_LOG2_TASK_NUM - 1 )
        else
        {
            for (pOS->n2_Dispatch = 0; pOS->n2_Dispatch<pOS->TaskRunningNum; pOS->n2_Dispatch++) //ѭ�����Ҿ���������  //---------�����㷨�Ż�--------//
            {
                pOS->_ReadyTaskIndex = pOS->TaskRunningList[pOS->n2_Dispatch];
                if (pOS->ReadyFlagGroup.all & ((lw_u64)1 << (pOS->_ReadyTaskIndex))) //���������־���ĳһλΪ1
                {
                    break;
                }//end if
            }//end for
        }//end else
				
				
#if	!OSLW_SIMPLE_MODE							
        pOS->LastTaskIndex = pOS->CurrentTaskIndex;
        pOS->pLastTask = pOS->pCurrentTask;//��¼��һ������
#endif
				
				
        pOS->CurrentTaskIndex = pOS->_ReadyTaskIndex;
        pOS->pCurrentTask = pOS->pTaskList[pOS->CurrentTaskIndex];//���±�������
        //pOS->pCurrentTask->TaskStatus=OSlwTaskStatus_Running;//��������״̬
				
        _OSLW_LOAD(pOS->pCurrentTask->BreakPoint[OSLW_TASK_BREAKPOINT_DEEP - 1], pOS->LastTaskIndex,pOS->pCurrentTask);//������ת

    }//end if
    else//˵����ǰû������׼������ ִ�п�������
    {
        
#if (OSLW_SIMPLE_MODE)			
				pOS->pCurrentTask=pOS->pTaskList[OSLW_TASK_NUM_MAX-1];
				break;
#else
			  pOS->pLastTask=pOS->pCurrentTask;
        pOS->LastTaskIndex=pOS->CurrentTaskIndex;
        pOS->pCurrentTask=pOS->pTaskList[OSLW_TASK_NUM_MAX-1];
        _OSLW_LOAD(pOS->pCurrentTask->BreakPoint[OSLW_TASK_BREAKPOINT_DEEP-1],pOS->LastTaskIndex,pOS->pCurrentTask);
#endif//(OSLW_SIMPLE_MODE)	
    }

		
		
		
		
#if (OSLW_SIMPLE_MODE)
	}while(1);
#endif//(OSLW_SIMPLE_MODE)					
		
    return;
}

//------------------------------------------
//<������>OSlwClockCallBack</������>
//<����˵��>����ϵͳʱ�ӽ��Ļص� �˺�������붨ʱ���ж�!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!</����˵��>
//<����˵��>pOS:this</����˵��>
//<���˵��>void</���˵��>
//------------------------------------------
void OSlwClockCallBack(OSlwCoreSTU *pOS)
{

    lw_u8 n=0;
    lw_u8 tindex=0;
    OSlwTaskSTU *pta;

    OSLW_assert(!pOS);

    if(pOS->CoreStatus!=OSlwCoreStatus_Running)
        return;
    pOS->NormalRunningTimerCount++;

	if (pOS->_CPU_Usage_count >= OSLW_CPU_USAGE_MAX)
	{
		pOS->_CPU_Usage_count = 0;
		pOS->CPU_Usage = pOS->_CPU_Usage_cal_reg;
		pOS->_CPU_Usage_cal_reg = OSLW_CPU_USAGE_MAX;
	}
	else
	{
		pOS->_CPU_Usage_count++;
		if (pOS->CurrentTaskIndex==OSLW_TASK_NUM_MAX-1)//����ǿ�������
		{
			pOS->_CPU_Usage_cal_reg--;//������ �����ȫ�ǿ������� �����������0
		}
	}

    for(; n<pOS->TaskRunningNum-1; n++) //ѭ�������б�
    {
        tindex=pOS->TaskRunningList[n];
        pta=pOS->pTaskList[tindex];//ȡ�����������
        //�ж�����״̬
        if(pta->TaskFrozenFlag) continue;//����Ƕ���״̬ ���ù�


		if (pta->BackToSleep.bits.timer_start_flag)
		{
			pta->BackToSleep.bits.work_time_count++;
		}

        switch((pta->TaskStatus))
        {
					
					
        case OSlwTaskStatus_Running:
#if !(OSLW_SIMPLE_LEVEL >= 3)
            pta->RunningTimeSinceStart++;
            pta->RunningTimeSinceRunning++;
						break;
#endif // !(OSLW_SIMPLE_LEVEL >= 3)
				
				
				
        case OSlwTaskStatus_Wait_ForSleep://�ȴ�˯��״̬
            if(!(--(pta->SleepCount)))//˯�߼Ĵ���-- ���Ϊ0��˵������ʱ�䵽��
            {
                OSlwCoreTaskIsReady(pOS,pta,tindex);//ת������״̬ Ϊ׼��̬
            }//end if
            break;
						
#if OSLW_GIFT_EN						
						
#if OSLW_SIMPLE_LEVEL < 3
        case OSlwTaskStatus_Wait_ForGiftReceive://�ȴ�����״̬
            if(pta->Concierge.giftR.List.NowLen > 1)//�������������Ϊ0 �Ϳ���׼������
            {
                OSlwCoreTaskIsReady(pOS,pta,tindex);//ת������״̬ Ϊ׼��̬
            }
            break;
#elif OSLW_SIMPLE_LEVEL == 3
        case OSlwTaskStatus_Wait_ForGiftReceive://�ȴ�����״̬
            if(pta->Concierge.pNext && pta->Concierge.pLast)//�������������Ϊ0 �Ϳ���׼������
            {
                OSlwCoreTaskIsReady(pOS,pta,tindex);//ת������״̬ Ϊ׼��̬
            }
            break;
						
#endif // !(OSLW_SIMPLE_LEVEL >= 4)						
						
#endif //OSLW_GIFT_EN						
						
            //�𽥷��� ����
#if 0
        case OSlwTaskStatus_Wait_ForGiftTransmit://�ȴ��������
            if(!(pta->Concierge.giftT.PostmanFlag.all))//�����־��==0 ����ȫ��������Ӧ���
            {
                OSlwCoreTaskIsReady(pOS,pta,tindex);//ת������״̬ Ϊ׼��̬
            }
            break;
#endif
						
						
						
						
        case OSlwTaskStatus_Wait_ForGroupFlag:
            if(pta->TaskGroupFlag.CurrentStatus.all & pta->TaskGroupFlag.AimStatus.all)
            {
                OSlwCoreTaskIsReady(pOS,pta,tindex);//ת������״̬ Ϊ׼��̬
            }
            break;
						
						
						
						
        default:
            break;


        }//end switch

    }//end for


    for(n=0; n<pOS->TimerCtrl.Length; n++)
    {
        OSlwTimerClockCallBack(pOS->TimerCtrl.pTimerList[n]);
    }

    pOS->pTaskList[OSLW_TASK_NUM_MAX - 1]->TaskGroupFlag.AimStatus.all = 0;



}



//------------------------------------------
//<������>OSlwCoreInitial</������>
//<����˵��>����ϵͳ�ں˳�ʼ������ ��ʼ���ں˼Ĵ��� ����ϵͳ�ڲ�����</����˵��>
//<����˵��>pOS:this</����˵��>
//<���˵��>void</���˵��>
//------------------------------------------
void OSlwCoreInitial(OSlwCoreSTU *pOS)
{
    int i;

    OSLW_assert(!pOS);
    memset(pOS,0,sizeof(OSlwCoreSTU));//�������
    memset(pOS->TaskRunningList,OSLW_TASK_NUM_MAX+1,sizeof(pOS->TaskRunningList));//ִ�б���ʼ��

	pOS->_CPU_Usage_cal_reg = OSLW_CPU_USAGE_MAX;

    //�����������ʼ����ֵ
#if OSLW_TASK_NUM_MAX<=8
    pOS->_mask_list[0] = 0x0f;

    pOS->_move_reg[0] = 4;

#elif OSLW_TASK_NUM_MAX>8 && OSLW_TASK_NUM_MAX<=16
    pOS->_mask_list[0] = 0x00ff;
    pOS->_mask_list[1] = 0x000f;

    pOS->_move_reg[0] = 8;
    pOS->_move_reg[1] = 4;
#elif OSLW_TASK_NUM_MAX>16 && OSLW_TASK_NUM_MAX<=32
    pOS->_mask_list[0] = 0x0000ffff;
    pOS->_mask_list[1] = 0x000000ff;
    pOS->_mask_list[2] = 0x0000000f;

    pOS->_move_reg[0] = 16;
    pOS->_move_reg[1] = 8;
    pOS->_move_reg[2] = 4;

#elif OSLW_TASK_NUM_MAX>32 && OSLW_TASK_NUM_MAX<=64
    pOS->_mask_list[0] = 0x00000000ffffffff;
    pOS->_mask_list[1] = 0x000000000000ffff;
    pOS->_mask_list[2] = 0x00000000000000ff;
    pOS->_mask_list[3] = 0x000000000000000f;

    pOS->_move_reg[0] = 32;
    pOS->_move_reg[1] = 16;
    pOS->_move_reg[2] = 8;
    pOS->_move_reg[3] = 4;

#else
#error "TASK NUM IS OVER"
#endif


    //����ָ���ʼ��
    pOS->StartFun = OSlwCoreStart;
    pOS->TimerAppendFun = OSlwTimerAppend;
    pOS->TaskAppendFun = OSlwTaskAppend;
    /*(Ver.=0.9~)(Beg.=0.9)
    #if !(OSLW_SIMPLE_MODE)
    pOS->ClockCBFun=OSlwClockCallBack;
    pOS->DispatchFun=OSlwTaskDispatch;
    pOS->TaskAppendFun=OSlwTaskAppend;
    pOS->StartFun=OSlwCoreStart;
    pOS->TimerAppendFun=OSlwTimerAppend;
    #endif
    */
    //�ڴ���Ƴ�ʼ��

//	//�������Ƴ�ʼ��
//	for(i=0;i<OSLW_CORE_PARA_CTRL_NUM;i++)
//	{
//
//		pOS->ParaCtrlList[i].pOS=pOS;
//#if !(OSLW_SIMPLE_MODE)
//		pOS->ParaCtrlList[i].CmdExeFun=OSlwParaCtrlCmdExe;
//#endif
//	}

    //�ڲ����� ��������

    OSlwTaskInit(//���������ʼ��
        &(pOS->OSlwInlineTaskBoring),
        OSLW_TASK_NUM_MAX-1,
        "~_~",
        0,
        (void *)0,
        OSlwInlineTaskBoringExe
    );
    OSlwTaskAppend(pOS, &(pOS->OSlwInlineTaskBoring));//��������׷��


    /*(Ver.=0.9~)(Beg.=0.9)
    	OSlwTaskInit(
    			&OSlwInlineTaskParameter,
    			1,
    			"T_T",
    			sizeof(OSlwParaCtrlSTU)*OSLW_CORE_PARA_CTRL_NUM,
    			(void *)(pOS->ParaCtrlList),
    			OSlwInlineTaskParameterExe
    			);
    	OSlwTaskAppend(pOS,&OSlwInlineTaskParameter);//������������
    */
/*(Ver.=0.9~)(Beg.=0.9)
    for(i=0; i<OSLW_CORE_PARA_PAGE_NUM; i++)
    {

        OSlwRealParaListInitial(&pOS->ParaBook[i],"");
        pOS->ParaBook[i].PageId=i;
        pOS->ParaBook[i].pOS=pOS;

    }
*/
#if OSLW_TASK_COMMUN_ENABLE //���ʹ��ͨ������
//	for(i=0;i<OSLW_CORE_COMMUN_LIST_LENGTH;i++)//ѭ��ͨ�ſ������б�
//	{
//		//ָ���ʼ��
//		pOS->Comm[i].pRx=pOS->Comm[i].RxList;
//		pOS->Comm[i].pTx=pOS->Comm[i].pTxTail=pOS->Comm[i].TxList;

//		//������ʼ��
//		pOS->Comm[i].ComRX.AppendFun=OSlwToolSlipwinAppend;
//		pOS->Comm[i].ComRX.MDataLen_forSize_t=(sizeof(lw_u8)*OSLW_COMMUN_RX_LENGTH);
//		pOS->Comm[i].ComRX.pBufferData=pOS->Comm[i].RxListBuf;
//		pOS->Comm[i].ComRX.pMasterData=pOS->Comm[i].RxList;

//		//����ָ���ʼ��
//		pOS->Comm[i].isReadyFun	= OSlwCommunIsReady;
//		pOS->Comm[i].TxReadyFun = OSlwCommunTxReady;
//
//		//����ϵͳָ��
//		pOS->Comm[i].pOS=pOS;
//		pOS->Comm[i].Address=OSLW_COMMUN_ADDRESS;//��ַ
//
//		//ͨ�ŷ���ģ���ʼ�� �����յ�����֮����ͨ��������
//		OSlwGiftTransmitInitial(&(pOS->Comm[i].giftT));
//		pOS->Comm[i].giftT.IdleIndexFun=OSlwGiftTransmitIdleIndex;
//	}
//
//	OSlwTaskInit(
//			&OSlwInlineTaskCommun,
//			2,
//			"^v^",
//			sizeof(OSlwCommunSTU),
//			(void *)(&(pOS->Comm)),
//			OSlwInlineTaskCommunExe
//			);
//	OSlwTaskAppend(pOS,&OSlwInlineTaskCommun);//ϵͳͨ������

#endif

#if OSLW_MEMORY_ENABLE
//�𽥷��� ����

#if 0
    OSlwMemoryMapInital(&(pOS->GiftMem),(pOS->GiftBuf), OSLW_CORE_GIFT_NUM,sizeof(OSlwGiftUnitSTU),sizeof(pOS->GiftBuf));
    OSlwMemoryMapInital(&(pOS->PostmanMem), (pOS->PostmanBuf), OSLW_CORE_POSTMAN_NUM, sizeof(OSlwGiftPostmanSTU), sizeof(pOS->PostmanBuf));
#endif


#endif




#if OSLW_TASK_SHOW_ENABLE

    pOS->Show.pOS=pOS;

    OSlwShowParaCtrlInitial(&(pOS->Show.PageList[0]),&(pOS->Show)
                            ,OSLW_SHOW_WIN_LENGTH,0,(void *)(&pOS->ParaCtrlList[0]),
                            OSlwShowParaCtrlCursorLock_Pvt,OSlwShowParaCtrlCursorUnLock_Pvt,OSlwShowParaCtrlCursorInput_Pvt
                            //NULL,NULL,NULL
                            ,NULL,NULL,OSlwDriverShowDrawCursor);

    OSlwShowParaCtrlInitial(&(pOS->Show.PageList[1]),&(pOS->Show),
                            OSLW_SHOW_WIN_LENGTH,0,(void *)(&pOS->ParaCtrlList[1]),
                            NULL,NULL,NULL,
                            NULL,NULL,OSlwDriverShowDrawCursor);

    pOS->Show.Cursor.pSPC=pOS->Show.PageList;

    pOS->Show.CursorMoveFun=OSlwShowCursorMoveFun;
    pOS->Show.DrawPageNumFun=OSlwDriverShowDrawPageNum;
    pOS->Show.InforProcFun=OSlwShowInforProc;
    pOS->Show.PageTrunFun=OSlwShowPageTrun;
    pOS->Show.UpdateFun=OSlwShowUpdate;

#if OSLW_SHOW_SCREEN == OSLW_SHOW_NO_KEEP

    OSlwTimerShowIrp.Mode=OSlwTimerMode_Continue;
    OSlwTimerShowIrp.Count=OSlwTimerShowIrp.CountMax=500/OSLW_CORE_TICK_MS;
    OSlwTimerShowIrp.AlarmCBFun=OSlwTimerShowIrpALFun;
    OSlwTimerShowIrp.pLeader=(void *)pOS;

    OSlwTimerShowIrp.ExtMem.Data=sizeof(pOS->Show.ShowBuffer);
    OSlwTimerShowIrp.ExtMem.pData=(void *)pOS->Show.ShowBuffer;

    OSlwTimerAppend(pOS,&OSlwTimerShowIrp);
#endif


    OSlwTimerShowUpdate.Mode=OSlwTimerMode_Continue;
    OSlwTimerShowUpdate.Count=OSlwTimerShowUpdate.CountMax=1000/OSLW_CORE_TICK_MS;
    OSlwTimerShowUpdate.AlarmCBFun=OSlwTimerShowUpdateALFun;
    OSlwTimerShowUpdate.pLeader=(void *)pOS;

    OSlwTimerShowUpdate.ExtMem.Data=sizeof(OSlwTaskSTU);
    OSlwTimerShowUpdate.ExtMem.pData=(void *)pta_Show;

    OSlwTimerAppend(pOS,&OSlwTimerShowUpdate);

    OSlwTaskInit(
        &OSlwInlineTaskShow,
        OSLW_TASK_NUM_MAX-2,
        "=v=",
        sizeof(OSlwShowSTU),
        (void *)(&(pOS->Show)),
        OSlwInlineTaskShowExe
    );
    OSlwTaskAppend(pOS,&OSlwInlineTaskShow);//ϵͳ��ʾ����



#endif

#if OSLW_SIMPLE_LEVEL<4 && OSLW_STR_DBG_EN

	OSlwStrDbgInitial(&(pOS->StrDbg));

	pOS->StrDbgFun = OSlwStrDbgFun;

#endif // OSLW_SIMPLE_LEVEL<4 && OSLW_STR_DBG_EN

}

//------------------------------------------
//<������>OSlwTaskAppend</������>
//<����˵��>����ϵͳ׷������ !!!!!!�ú�������������ƿ�ĳ�ʼ��!!!!!!!!!</����˵��>
//<����˵��>pOS:this pta:׷��������ƿ�</����˵��>
//<���˵��>pOS:this</���˵��>
//------------------------------------------
OSlwCoreSTU *OSlwTaskAppend(OSlwCoreSTU *pOS,OSlwTaskSTU *pta)
{
    OSLW_assert(!pOS);
    OSLW_assert(!pta);

    if(pOS->pTaskList[pta->Priority])//�жϸ�������Ƿ��Ѿ���ע��
    {
        pOS->CoreError=OSlwCoreError_PriorityRedefine;//�ں��쳣����
        return pOS;//����
    }

    pta->pOS=pOS;//���������Ĳ���ϵͳָ��


    pOS->pTaskList[pta->Priority]=pta;//������ƿ������Ӧλ��
    pOS->TaskRunningList[pOS->TaskRunningNum]=pta->Priority;//�������ȼ����������б���
    pOS->TaskRunningNum++;//����������+1
    _quicksort(pOS->TaskRunningList,pOS->TaskRunningNum,sizeof(pOS->TaskRunningList[0]),comp);//�����б���������� ��С���
    pOS->InitialFlagGroup.all|=((lw_u64)1<<pta->Priority);//�ں˳�ʼ����־������

    return pOS;
}

//------------------------------------------
//<������>OSlwParaAppend</������>
//<����˵��>����׷������ ��OS׷�Ӳ���</����˵��>
//<����˵��>pOS:this num:�����б���� ������0(����Ĵ�������) 1(���ּĴ�������) *p:����ָ��</����˵��>
//<���˵��>this</���˵��>
//------------------------------------------
/*(Ver.=0.9~)(Beg.=0.9)
OSlwCoreSTU* OSlwParaAppend(OSlwCoreSTU *pOS,lw_u8 num,struct OSLW_PARAMETER_STRUCT *p)
{

	OSlwParaCtrlSTU *pPC;

	OSLW_assert(!pOS);
	OSLW_assert(!p);

	num= num % OSLW_CORE_PARA_CTRL_NUM;//% ��֤�б���Ų���Խ��
	pPC = & (pOS->ParaCtrlList[num]);// ȡ��ϵͳ���������б���ָ��
	if (pPC->ParaLength >= OSLW_PARA_LIST_LENGRH)//������������Ѿ������б��ܳ���
	{
		pOS->CoreError=OSlwCoreError_ParaListLengthOver;//���ô����־
		return pOS;
	}
	p->ParaId=pPC->ParaLength;//���ò������
	pPC->pParaList[pPC->ParaLength]=p;//�������
	pPC->ParaLength++;//����+1

	return pOS;

}
*/


/*(Ver.=0.9~)(Beg.=0.9)
OSlwCoreSTU* OSlwParaAppend(OSlwCoreSTU *pOS,lw_u8 PageId,OSlwParameterSTU *pP)
{
    OSlwParaListCtrlSTU *pPLC;

    OSLW_assert(!pOS);
    OSLW_assert(!pP);

    PageId= PageId % OSLW_CORE_PARA_PAGE_NUM;
    pPLC = & (pOS->ParaBook[PageId]);// ȡ��ϵͳ���������б���ָ��

    if (pPLC->ParaLength >= OSLW_PARA_LIST_LENGRH)//������������Ѿ������б��ܳ���
    {
        pOS->CoreError=OSlwCoreError_ParaListLengthOver;//���ô����־
        return pOS;
    }

    pP->ParaId=pOS->ParaIdCount++;
    pPLC->pParaList[pPLC->ParaLength]=pP;
    pPLC->ParaLength++;

    return pOS;

}
*/

OSlwCoreSTU* OSlwTimerAppend(OSlwCoreSTU *pOS,OSlwTimerSTU *pT)
{
    if(pOS->TimerCtrl.Length >= OSLW_TIMER_CTRL_LIST_LEN)
    {
        pOS->CoreError=OSlwCoreError_TimerListLengthOver;
        return NULL;
    }

    pT->ID=pOS->TimerCtrl.Length;

    pOS->TimerCtrl.pTimerList[pOS->TimerCtrl.Length++]=pT;

    return pOS;

}


//------------------------------------------
//<������>OSlwCoreStart</������>
//<����˵��>OSLW��ʼ !!!!!��������������ƿ������ϵͳ�ں˳�ʼ�����֮���ٵ���!!!!!!</����˵��>
//<����˵��>pOS:this</����˵��>
//<���˵��>void</���˵��>
//------------------------------------------
void OSlwCoreStart(OSlwCoreSTU *pOS)
{

    pOS->DispatchEable=1;//������ʹ��
    pOS->CoreStatus=OSlwCoreStatus_Running;//�����ں�ת��Ϊ����̬
#if !(OSLW_SIMPLE_MODE)
    OSlwTaskDispatch(pOS);//ִ���������
#else
    pOS->OSlwInlineTaskBoring.TaskFun(&(pOS->OSlwInlineTaskBoring));
#endif
    while(1)//!!!!!!!!!!!!��������²���������ѭ��!!!!!!!!!!!!!!!!!!!!!
    {
        ;
    }

}




//------------------------------------------
//<������>comp</������>
//<����˵��>�ȽϺ��� �������ȼ�����</����˵��>
//------------------------------------------
lw_16 comp(const void*a,const void*b)
{

    return (*(lw_u8*)a-*(lw_u8*)b);

}

//------------------------------------------
//<������>_quicksort</������>
//<����˵��>��������</����˵��>//
//by ������
//------------------------------------------
void _quicksort (void *const pbase, size_t total_elems, size_t size,_compar_fn cmp)
{
    char *base_ptr = (char *) pbase;

    const size_t max_thresh = MAX_THRESH * size;

    if (total_elems == 0)
        /*(Ver.=0.9~)(Beg.=0.9) Avoid lossage with unsigned arithmetic below.  */
        return;

    if (total_elems > MAX_THRESH)
    {
        char *lo = base_ptr;
        char *hi = &lo[size * (total_elems - 1)];
        stack_node stack[STACK_SIZE];
        stack_node *top = stack;

        PUSH (NULL, NULL);

        while (STACK_NOT_EMPTY)
        {
            char *left_ptr;
            char *right_ptr;
            char *mid = lo + size * ((hi - lo) / size >> 1);

            if ((*cmp) ((void *) mid, (void *) lo) < 0)
                SWAP (mid, lo, size);
            if ((*cmp) ((void *) hi, (void *) mid) < 0)
                SWAP (mid, hi, size);
            else
                goto jump_over;
            if ((*cmp) ((void *) mid, (void *) lo) < 0)
                SWAP (mid, lo, size);
jump_over:
            ;

            left_ptr  = lo + size;
            right_ptr = hi - size;
            do
            {
                while ((*cmp) ((void *) left_ptr, (void *) mid) < 0)
                    left_ptr += size;

                while ((*cmp) ((void *) mid, (void *) right_ptr) < 0)
                    right_ptr -= size;

                if (left_ptr < right_ptr)
                {
                    SWAP (left_ptr, right_ptr, size);
                    if (mid == left_ptr)
                        mid = right_ptr;
                    else if (mid == right_ptr)
                        mid = left_ptr;
                    left_ptr += size;
                    right_ptr -= size;
                }
                else if (left_ptr == right_ptr)
                {
                    left_ptr += size;
                    right_ptr -= size;
                    break;
                }
            }
            while (left_ptr <= right_ptr);

            if ((size_t) (right_ptr - lo) <= max_thresh)
            {
                if ((size_t) (hi - left_ptr) <= max_thresh)
                    /*(Ver.=0.9~)(Beg.=0.9) Ignore both small partitions. */
                    POP (lo, hi);
                else
                    /*(Ver.=0.9~)(Beg.=0.9) Ignore small left partition. */
                    lo = left_ptr;
            }
            else if ((size_t) (hi - left_ptr) <= max_thresh)
                /*(Ver.=0.9~)(Beg.=0.9) Ignore small right partition. */
                hi = right_ptr;
            else if ((right_ptr - lo) > (hi - left_ptr))
            {
                /*(Ver.=0.9~)(Beg.=0.9) Push larger left partition indices. */
                PUSH (lo, right_ptr);
                lo = left_ptr;
            }
            else
            {
                /*(Ver.=0.9~)(Beg.=0.9) Push larger right partition indices. */
                PUSH (left_ptr, hi);
                hi = right_ptr;
            }
        }
    }

#define min(x, y) ((x) < (y) ? (x) : (y))
    {
        char *const end_ptr = &base_ptr[size * (total_elems - 1)];
        char *tmp_ptr = base_ptr;
        char *thresh = min(end_ptr, base_ptr + max_thresh);
        char *run_ptr;
        for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
            if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
                tmp_ptr = run_ptr;

        if (tmp_ptr != base_ptr)
            SWAP (tmp_ptr, base_ptr, size);

        run_ptr = base_ptr + size;
        while ((run_ptr += size) <= end_ptr)
        {
            tmp_ptr = run_ptr - size;
            while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
                tmp_ptr -= size;

            tmp_ptr += size;
            if (tmp_ptr != run_ptr)
            {
                char *trav;

                trav = run_ptr + size;
                while (--trav >= run_ptr)
                {
                    char c = *trav;
                    char *hi, *lo;

                    for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                        *hi = *lo;
                    *hi = c;
                }
            }
        }
    }
}

#endif
