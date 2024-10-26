include "base.thrift"
include "cv_types.thrift"

namespace py cv_common
namespace go cv_common
namespace cpp cv_common
namespace js cv_common

/* 道玄: Services:
* 1. Classification
* 2. Segmentation
* 3. Detection
* 4. Recognition
* 5. Joints Estimation
* 6. Inpainting
* 7. Get Model Version */
service VisionServices {
    cv_types.VideoTagPredictRsp Predict(1: cv_types.VideoPredictReq req) throws (1: cv_types.VisionException exp),
    cv_types.ImageSegRsp Seg(1: cv_types.ImagePredictReq req),
    cv_types.ImageDetRsp Detect(1: cv_types.ImagePredictReq req),
    cv_types.ImageFaceRsp Reg(1: cv_types.ImagePredictReq req),
    cv_types.ImageJointRsp Est(1: cv_types.ImagePredictReq req),
    cv_types.ImagesInpaintRsp Inpaint(1: cv_types.ImagesInpaintReq req),
    cv_types.ModelVersionRsp GetModelVersion(1: cv_types.ModelVersionReq req),
    cv_types.ImageJsonRsp RunJson(1: cv_types.ImageJsonReq req),
}
