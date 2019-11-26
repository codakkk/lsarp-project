
static enum _:TaxType
{
    TAX_TYPE_VAT,
    TAX_TYPE_TOBIN,
    TAX_TYPE_BIZ,
    TAX_TYPE_VEHICLE,
    TAX_TYPE_IMMATRICOLAZIONE
}

static enum TaxTypeData
{
    Type,
    Name[32],
    DefaultValue,
    CurrentValue
};

static Map:Taxes;

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    Taxes = map_new();
    DefineTaxType(TAX_TYPE_VAT, "Value Added Tax", 0);
    DefineTaxType(TAX_TYPE_TOBIN, "Tobin Tax", 0);
    DefineTaxType(TAX_TYPE_TOBIN, "Tassa Business", 0);
    DefineTaxType(TAX_TYPE_TOBIN, "Tassa Veicolo", 0);
    DefineTaxType(TAX_TYPE_TOBIN, "Tassa Immatricolazione", 0);

    return 1;
}

static DefineTaxType(tax_type, const taxName[], defaultValue)
{
    new data[TaxTypeData];
    data[Type] = tax_type;
    data[DefaultValue] = defaultValue;
    set(data[Name], taxName);
    data[CurrentValue] = defaultValue;
    map_add_arr(Taxes, tax_type, taxName);
}

stock Tax_GetValue(taxType)
{
    if(!map_has_key(Taxes, taxType))
        return 0;
    return map_get(Taxes, taxType, CurrentValue);
}

stock bool:Tax_SetValue(taxType, newValue)
{
    if(!map_has_key(Taxes, taxType))
        return false;
    return map_set_cell(Taxes, taxType, CurrentValue, newValue);
}

stock Tax_CalculateByValue(taxType, baseValue)
{
    return baseValue + floatround(float(baseValue * Tax_Getvalue(taxType))/100.0, floatround_ceil);
}

static LoadTaxes()
{
    await mysql_aquery_s(gMySQL, str_format("SELECT type, current_value FROM taxes"), .parallel=true);
    
    new rows = cache_num_rows(), data[TaxTypeData];
    for(new i = 0; i < rows; ++i)
    {
        cache_get_value_name_int(i, "type", data[Type]);
        if(!map_has_key(Taxes, data[Type]))
        {
            printf("Trying to load an undefined tax type.");
            continue;
        }
        map_get_arr(Taxes, data[Type], data);
        cache_get_value_name_int(i, "current_value", data[CurrentValue]);
        map_set_arr(Taxes, data[Type], data);
    }
}