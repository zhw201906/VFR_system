//���ڽ���VZ���ǻۺ��ӵ�Ӧ�ó���ӿ�
#ifndef _VZ_BOX_CLIENT_SDK_
#define _VZ_BOX_CLIENT_SDK_
#include <VzClientSDKDefine.h>



#ifdef  LINUX
#ifdef __cplusplus
extern "C"
{
#endif
#endif

/**
*  @brief ��ȡ�������ӵ�����б�
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [OUT] cam_group �����Ϣ�б�
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_GetCamGroupParam(VzClientHandle handle, VZ_BOX_CAM_GROUP* cam_group);

/**
*  @brief ������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] pInfo ���ͨ������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_BoxAddCam(VzClientHandle handle, const VZ_BOX_CAM_INFO *pInfo);

/**
*  @brief ����ͨ���ű༭���������޸���IP���ԣ�����Ƴ����з��飬��Ҫ�������÷�����Ϣ
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] nChnId ��VzClient_GetCamGroupParam������ȡ����Ϣ��VZ_BOX_CAM_GROUP::VZ_BOX_CAM_ITEM::chn_id
*  @param [IN] pInfo ���ͨ������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_BoxSetCam(VzClientHandle handle, int nChnId, const VZ_BOX_CAM_INFO *pInfo);

/**
*  @brief ����IP��ַ����ɾ�������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] ip ��ɾ�������IP��ַ����
*  @param [IN] nNum ��ɾ�������IP��ַ���鳤��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_BoxRemoveCams(VzClientHandle handle, const char * const ip[], int nNum);

/**
*  @brief ��ƽ̨�������Ա�
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] app_id ������ID����
*  @param [IN] app_count ���������
*  @param [IN] image_data ����ͼƬ����(jpg��ʽ�ļ�����)
*  @param [IN] image_len  ͼƬ���ݵĳ���
*  @param [IN] max_user_num  �����󷵻صĽ������
*  @param [IN] detail_type  �����󷵻صĽ������(0��������ͼƬ���û���ϸ��Ϣ 1������ͼƬ���û���ϸ��Ϣ)
*  @param [OUT] face_result �����ԱȽ��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_CloudFaceRecognition(VzClientHandle handle, int* app_id, int app_count, char *image_data, int image_len, int max_user_num, int detail_type, VZ_FACE_USER_RESULT *face_result);


/**
*  @brief ���������ԱȽ�����ɵ�ͼƬ����(�����˽ӿ�����������ڴ�й¶)
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_ReleaseFaceRecognitionData(VzLPRClientHandle handle, VZ_FACE_USER_RESULT *face_result);

/**
*  @brief ��ƽ̨������ʶ��
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] app_id ���ƿ�ID
*  @param [IN] image_data ����ͼƬ����(jpg��ʽ�ļ�����)
*  @param [IN] image_len  ͼƬ���ݵĳ���
*  @param [OUT] plate_result ����ʶ����
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_CloudPlateRecognition(VzClientHandle handle, int app_id, char *image_data, int image_len, TH_PlateResult *plate_result);


/**
*  @brief ����ʶ������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] item ��������Ϣ
*  @param [IN] oper ��������(1�������׿� 2��ɾ���׿� 3���޸ĵ׿�)
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgLibOperate(VzClientHandle handle, VZ_FACE_LIB_ITEM *item, int oper);


/**
*  @brief ����������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [OUT] lib_result ��������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_SearchFaceRecgLib(VzClientHandle handle, VZ_FACE_LIB_RESULT *lib_result);

/**
*  @brief ���������Ϣ������ʶ���
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] user_info �û�����
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgUserAdd(VzClientHandle handle, VZ_FACE_USER_RESULT *user_info);

/**
*  @brief �༭������Ϣ����Ҫ����pic_index��
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] user_info �û�����
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgUserEdit(VzClientHandle handle, VZ_FACE_USER_RESULT *user_info);

/**
*  @brief ɾ����������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] pic_index ��������id
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgUserDelete(VzClientHandle handle, int pic_index);


/**
*  @brief ����������ݵ�������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] user_item ��������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgAddUserToArray(VzClientHandle handle, VZ_FACE_USER_ITEM *user_item);

/**
*  @brief ��������е�����
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] user_item ��������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgClearUserArray(VzClientHandle handle);

/**
*  @brief ִ����������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] user_item ��������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgBatchImportUserFromArray(VzClientHandle handle);

/**
*  @brief ��ȡ������
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [OUT] success_count �ɹ��ĸ���
*  @param [OUT] failed_count  ʧ�ܵĸ���
*  @param [OUT] total_count   ����ĸ���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgGetImportResult(VzClientHandle handle, int *import_count, int *total_count);


/**
*  @brief ��ȡ����Ĵ�����Ϣ
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [OUT] err_msg �ɹ��ĸ���
*  @param [IN]  max_msg_len  ʧ�ܵĸ���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_FaceRecgGetImportErrMsg(VzClientHandle handle, char *err_msg, int max_len);

/**
*  @brief ��������ʶ����е�����
*  @param [IN] handle ��VzClient_Open������õľ��
*  @param [IN] condition ��ѯ����
*  @param [IN] db_id �������id
*  @param [OUT] user_info ��ѯ���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_SearchFaceRecgUser(VzClientHandle handle, VZ_FACE_LIB_SEARCH_CONDITION *condition, int db_id, VZ_FACE_USER_RESULT *user_info);

/**
*  @brief ����·����ȡͼƬ
*  @param  [IN] handle ��VzLPRClient_Open������õľ��
*  @param  [IN] img_path �洢����������ͼƬ·��
*  @param  [IN] pdata  �洢ͼƬ���ڴ�
*  @param  [IN][OUT] size Ϊ���봫��ֵ������ΪͼƬ�ڴ�Ĵ�С�����ص��ǻ�ȡ��jpgͼƬ�ڴ�Ĵ�С
*  @return ����ֵΪ0��ʾ�ɹ�������-1��ʾʧ��
*  @ingroup group_device
*/
VZ_API int __STDCALL VzClient_LoadFaceImageByPath(VzClientHandle handle, const char* img_path, void *pdata, int* size);


/**
*  @brief ��ȡ���ץ�ĵ�һ��������ߵĽ��
*  @param  [IN] handle ��VzLPRClient_Open������õľ��
*  @param  [IN] near_time  ʱ�䷶Χ
*  @param  [OUT] face_result �������
*  @return ����ֵΪ0��ʾ�ɹ�������-1��ʾʧ��
*  @ingroup group_device
*/
VZ_API int __STDCALL VzClient_GetBestFaceResult(VzClientHandle handle, int near_time, TH_FaceResult* face_result);


/**
*  @brief ����������ȡ������ߵ�ͼƬ����(�����˽ӿ�����������ڴ�й¶)
*  @param [IN] face_result ��VzClient_GetBestFaceResult���صĽ��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_ReleaseBestFaceResult(TH_FaceResult *face_result);

/**
*  @brief ��ȡϵͳ��ǰ����������ȡ���ص�״̬
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [OUT] feature_extract_status ����������ȡ״̬�� 0��ʾδ����������ȡ��Ĭ�ϣ���1��ʾ����������ȡ
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_GetFaceFeatureExtractStatus(VzClientHandle handle, int *feature_extract_status);

/**
*  @brief ����ϵͳ����������ȡ����
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] feature_extract ����������ȡ״̬�� 0��ʾ������������ȡ��Ĭ�ϣ���1��ʾ����������ȡ
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_SetFaceFeatureExtractStatus(VzClientHandle handle, int feature_extract);

/**
*  @brief ��ȡϵͳ��ǰ����������ȡ���ص�״̬
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [OUT] feature_extract_status ����������ȡ״̬�� 0��ʾδ����������ȡ��Ĭ�ϣ���1��ʾ����������ȡ
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_BoxGetSystemModuleSwitch(VzClientHandle handle, 
													   VZ_BOX_SYS_MOD_SWITCH eSwitch,
													   unsigned char *ucEnable);

/**
*  @brief ����ϵͳ����������ȡ����
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] feature_extract ����������ȡ״̬�� 0��ʾ������������ȡ��Ĭ�ϣ���1��ʾ����������ȡ
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ�ܡ�
*/
VZ_API int __STDCALL VzClient_BoxSetSystemModuleSwitch(VzClientHandle handle, 
													VZ_BOX_SYS_MOD_SWITCH eSwitch,
													unsigned char ucEnable);

/**
*  @brief  ͨ���ûص�������ñ�������
*  @param  [IN] handle		��VzLPRClient_Open������õľ��
*  @param  [IN] playhandle  ���ž��(VzClient_SetRealDataCallBack�������صľ��)
*  @param  [IN] pUserData	�ص�������������
*  @param  [IN] eDataType	�������ͣ����ö��VZ_LPRC_DATA_TYPE
*  @param  [IN] pData		����֡��Ϣ������ṹ�嶨��VZ_LPRC_DATA_INFO
*  @ingroup group_callback
*/
typedef void (__STDCALL *VZ_REAL_DATA_CALLBACK)(VzClientHandle handle, int channel, int playhandle, void *pUserData,
													 VZ_LPRC_DATA_TYPE eDataType, const VZ_LPRC_DATA_INFO *pData);
#ifdef WIN32
/**
*  @brief ��ȡʵʱ��������
*  @param  [IN] handle		��VzLPRClient_Open������õľ��
*  @param  [IN] channel     ͨ����
*  @param  [IN] stream_id   ����id(0������, 1������)
*  @param  [IN] func		�ص�����
*  @param  [IN] pUserData	�ص������е�������
*  @return ���ŵľ����-1��ʾʧ��
*  @ingroup group_device
*/
VZ_API int __STDCALL VzClient_SetChannelRealDataCallBack(VzLPRClientHandle handle,
												  int channel, 
												  int stream_id,
												  VZ_REAL_DATA_CALLBACK func, 
												  void *pUserData);
#endif

/**
*  @brief �豸˫���ڵ�����������á�
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] netcfg �μ�VZ_LPRC_NETCFG_EX�ṹ�嶨��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_SetSecondPortNetConfig(VzClientHandle handle, const VZ_LPRC_NETCFG_EX* netcfg);

/**
*  @brief ��ȡ˫���ڵ�����������á�
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] netcfg �μ�VZ_LPRC_NETCFG_EX�ṹ�嶨��
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_GetSecondPortNetConfig(VzClientHandle handle, VZ_LPRC_NETCFG_EX* netcfg);
/**
*  @brief ����http���ͻ�������
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] basecfg ���ͻ������ò���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_SetHttpPushBaseConfig(VzClientHandle handle, VZ_HTTP_PUSH_BASE_CFG* basecfg);

/**
*  @brief ��ȡhttp���ͻ�������
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] basecfg ���ͻ������ò���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_GetHttpPushBaseConfig(VzClientHandle handle, VZ_HTTP_PUSH_BASE_CFG* basecfg);

/**
*  @brief ����http���ͽ������
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] push_param ���ͽ������
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_SetBoxHttpPushParam(VzClientHandle handle, VZ_BOX_HTTP_PUSH_PARAM* push_param);

/**
*  @brief ��ȡhttp���ͽ������
*  @param [IN] handle ��VzLPRClient_Open������õľ��
*  @param [IN] push_param �������ò���
*  @return ����ֵΪ0��ʾ�ɹ�����������ֵ��ʾʧ��
*/
VZ_API int __STDCALL VzClient_GetBoxHttpPushParam(VzClientHandle handle, VZ_BOX_HTTP_PUSH_PARAM* push_param);



#ifdef  LINUX
#ifdef __cplusplus
}
#endif
#endif


#endif
