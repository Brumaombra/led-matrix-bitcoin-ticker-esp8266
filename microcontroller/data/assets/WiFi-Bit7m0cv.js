import{r as d,c as r,b as e,g as b,h as u,v as y,F as v,j as h,u as x,k,G as g,l as S,m as c,o,t as p,s as f,p as F}from"./index-BvpICkRu.js";const _="/wifi.svg",C={class:"flex justify-center"},D={class:"max-w-md w-full bg-white p-8 rounded-lg shadow-lg"},E={class:"mb-4"},B={class:"flex items-center"},L=["value"],M={key:0},W={key:1},A={class:"mb-4"},V={__name:"WiFi",setup(I){const n=d(""),l=d(""),i=d(!1),m=async()=>{try{f(!0),await F(n.value,l.value),c("Success","Success","You have successfully connected the device to the Wi-Fi network! The access point will be disabled. Enjoy the Bitcoin ticker!")}catch(a){console.error(a),c("Error","Error","An error occurred while connecting to the Wi-Fi network, try with a different network or check the password")}finally{f(!1),l.value=""}},w=async()=>{i.value=!0;try{g.networksList=await S()}catch(a){console.error(a),c("Error","Error","An error occurred while refreshing the SSID list")}finally{i.value=!1}};return(a,s)=>(o(),r("div",C,[e("div",D,[s[7]||(s[7]=e("div",{class:"mb-6"},[e("div",{class:"flex justify-center mb-6"},[e("img",{src:_,alt:"Bitcoin logo",class:"h-16 w-16"})]),e("h1",{class:"text-3xl font-bold text-center text-gray-800"},"Connect to WiFi")],-1)),e("form",{onSubmit:b(m,["prevent"])},[e("div",E,[s[4]||(s[4]=e("label",{for:"ssid",class:"block text-sm font-medium text-gray-700 mb-2"},"SSID",-1)),e("div",B,[u(e("select",{"onUpdate:modelValue":s[0]||(s[0]=t=>n.value=t),id:"ssid",name:"ssid",class:"block w-full px-4 pr-10 py-3 text-gray-900 bg-white border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500 appearance-none bg-[url('data:image/svg+xml;charset=US-ASCII,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%2220%22%20height%3D%2220%22%20viewBox%3D%220%200%2020%2020%22%3E%3Cpath%20fill%3D%22%23666%22%20d%3D%22M5.293%208.293a1%201%200%20011.414%200L10%2011.586l3.293-3.293a1%201%200%20111.414%201.414l-4%204a1%201%200%2001-1.414%200l-4-4a1%201%200%20010-1.414z%22%2F%3E%3C%2Fsvg%3E')] bg-[length:20px] bg-[right_12px_center] bg-no-repeat hover:cursor-pointer"},[s[2]||(s[2]=e("option",{value:""},"Select a network",-1)),(o(!0),r(v,null,h(x(g).networksList,t=>(o(),r("option",{key:t.ssid,value:t.ssid},p(t.ssid)+" ("+p(t.signal)+" dBm) ",9,L))),128))],512),[[y,n.value]]),e("button",{type:"button",onClick:w,class:"ml-2 px-4 py-3 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600"},[i.value?(o(),r("div",M,s[3]||(s[3]=[e("svg",{class:"animate-spin h-5 w-5 text-white",xmlns:"http://www.w3.org/2000/svg",fill:"none",viewBox:"0 0 24 24"},[e("circle",{class:"opacity-25",cx:"12",cy:"12",r:"10",stroke:"currentColor","stroke-width":"4"}),e("path",{class:"opacity-75",fill:"currentColor",d:"M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"})],-1)]))):(o(),r("span",W,"Refresh"))])])]),e("div",A,[s[5]||(s[5]=e("label",{for:"password",class:"block text-sm font-medium text-gray-700 mb-2"},"Password",-1)),u(e("input",{"onUpdate:modelValue":s[1]||(s[1]=t=>l.value=t),type:"password",id:"password",name:"password",class:"block w-full px-4 py-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-gray-500 focus:border-gray-500",placeholder:"Enter your WiFi password"},null,512),[[k,l.value]])]),s[6]||(s[6]=e("button",{type:"submit",class:"w-full py-3 px-4 border border-transparent rounded-lg text-sm font-medium text-white bg-gray-800 hover:bg-gray-600"}," Connect ",-1))],32)])]))}};export{V as default};
