function initPage()
{
	//head text
	var head_tag = document.getElementsByTagName("h1");
	var head_text = document.createTextNode(bh_apply_connection);
	head_tag[0].appendChild(head_text);
	
	//paragraph
	var paragraph = document.getElementsByTagName("p");
	
	var paragraph_text = document.createTextNode(bh_plz_waite_apply_connection);
	paragraph[0].appendChild(paragraph_text);

	//show Fireware Version
	showFirmVersion("none");
	if(ap_mode =="1")
	{
		if(getTop(window).location.href.indexOf("BRS_index.htm") > -1){
			if(getTop(window).location.href.indexOf("routerlogin.net") > -1){
				setTimeout("getTop(window).location.href='http://routerlogin.com/BRS_index.htm';", 100000);
			}else{
				setTimeout("getTop(window).location.href='http://routerlogin.net/BRS_index.htm';", 100000);
			}
		}else{
				setTimeout("this.location.href='BRS_04_applySettings_ping.html';", 100000);
		}
	}
	else
	{
		if(getTop(window).dsl_enable_flag == "1")
		{
			if(getTop(window).location.href.indexOf("BRS_index.htm") > -1){
				if(getTop(window).location.href.indexOf("routerlogin.net") > -1){
					//setTimeout("getTop(window).location.href='http://routerlogin.com/BRS_index.htm';", 10000);
					setTimeout("set_jump(1)", 20000);
				}else{
					//setTimeout("getTop(window).location.href='http://routerlogin.net/BRS_index.htm';", 10000);
					setTimeout("set_jump(2)", 20000);
				}
			}else{
				setTimeout("this.location.href='BRS_04_applySettings_ppp_obtain_ip.html';", 30000);
			}
		}else{
			if(getTop(window).location.href.indexOf("BRS_index.htm") > -1 && (getTop(window).support_orange_flag != 1 && getTop(window).orange_apply_flag != "1")){
				if(getTop(window).location.href.indexOf("routerlogin.net") > -1){
				if(getTop(window).support_orange_flag ==1)
					setTimeout("getTop(window).location.href='http://routerlogin.com/BRS_index.htm';", 75000);
				else
					setTimeout("getTop(window).location.href='http://routerlogin.com/BRS_index.htm';", 60000);
				}else{
				if(getTop(window).support_orange_flag ==1)
					setTimeout("getTop(window).location.href='http://routerlogin.net/BRS_index.htm';", 75000);
				else
					setTimeout("getTop(window).location.href='http://routerlogin.net/BRS_index.htm';", 60000);
				}
			}else
				if(getTop(window).support_orange_flag ==1){
				setTimeout("this.location.href='BRS_04_applySettings_ping.html';", 75000);
				getTop(window).orange_apply_flag = "0";
				}
				else
				setTimeout("this.location.href='BRS_04_applySettings_ping.html';", 60000);
		}
	}
}


function set_jump(num)
{
	var tHttp = createXMLHttpRequest();
	var url = "/obtain_ip.txt?ts=" + new Date().getTime();
	tHttp.onreadystatechange = function()
	{
		if(tHttp.readyState == 4 && tHttp.status == 200)
		{
			if(num == 1)
				getTop(window).location.href='http://routerlogin.com/BRS_index.htm';
			else	
				getTop(window).location.href='http://routerlogin.net/BRS_index.htm';
		} else {
			  if ( tHttp.readyState == 4 && tHttp.status == 0 )
			  {	
					if(num == 1)
						setTimeout("getTop(window).location.href='http://routerlogin.com/BRS_index.htm';", 20000);
					else
						setTimeout("getTop(window).location.href='http://routerlogin.net/BRS_index.htm';", 20000);
				 
				
			  }
		}
  }
  tHttp.open("GET",url,true);
  tHttp.send(null);
}
addLoadEvent(initPage);