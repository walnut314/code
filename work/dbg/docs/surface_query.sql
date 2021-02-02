// 
// goto this link and paste the below query
// https://dataexplorer.azure.com/clusters/surfacedtel/databases/PLEReporting
//
// Join devices with ABS event with MCU on SLG Sparti.
let Models = pack_array(
//    'Surface Pro 7' 
//    'Surface Laptop 3 13in'
    'Surface Laptop Go' 
//    'Surface Book 3 13in'
);
let a = McuHealthLog
| where __date > ago(3d) and OEMDerivedModel in (Models) and Target  == "SAM"
| project OEMSerialNumber, __date, OEMDerivedModel
| distinct OEMSerialNumber, __date, OEMDerivedModel
| order by  OEMDerivedModel asc, OEMSerialNumber asc
| extend rank = row_number(1, prev(OEMDerivedModel) != OEMDerivedModel and prev(OEMSerialNumber) != OEMSerialNumber)
| where rank <= 5000;
let b = ABSEvaluatedData
| where OEMDerivedModel in (Models) and RetailChannel  == "Retail" and __date > ago(90d)
//| where isnotempty( ABSDetailedReason )
| where ABSDetailedReason == "Unknown"
| distinct OEMSerialNumber, __date;
let c = a| join b  on  OEMSerialNumber   // devices has ABS (Unknown reason) in last 90 days 
| distinct OEMSerialNumber, __date1
| extend __date = __date1;
c | join McuHealthLog on OEMSerialNumber, __date 
| project __date, OEMSerialNumber, HealthLogDecoded, ReleaseTag
| evaluate  bag_unpack(HealthLogDecoded)
| project __date, OEMSerialNumber, ReleaseTag, ["HLTH_CATERR_FAIL"], ["HLTH_HARD_FAULT"],["HLTH_OS_ASSERT"]
| order by OEMSerialNumber asc, __date asc, ["HLTH_CATERR_FAIL"] asc
| where tolong(['HLTH_CATERR_FAIL']) > 0
//| count

